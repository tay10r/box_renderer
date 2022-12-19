#include "Renderer.h"

#include "Scene.h"

#include <bvh/v2/stack.h>
#include <bvh/v2/executor.h>
#include <bvh/v2/thread_pool.h>
#include <bvh/v2/default_builder.h>

#include <random>

#include <cmath>

namespace mcr {

namespace {

struct Material final
{
  const Texture* texture{ nullptr };

  Scalar light_scale{ 0 };
};

struct Job final
{
  const int width{ 0 };

  const int height{ 0 };

  std::vector<Texture::Color> color;

  int samples{ 0 };

  Job(const int w, const int h)
    : width(w)
      , height(h)
      , color(initialize_colors(w * h))
  {
  }

private:
  static std::vector<Texture::Color> initialize_colors(const int count)
  {
    std::vector<Texture::Color> colors(count);

    for (int i = 0; i < count; i++)
      colors[i] = Texture::Color(0.0, 0.0, 0.0, 1.0);

    return colors;
  }
};

class RendererImpl final : public Renderer
{
public:
  explicit RendererImpl(const SceneData& scene_data)
    : scene_data_(&scene_data)
  {
    for (const auto& camera : scene_data_->cameras)
      jobs_.emplace_back(camera.resolution_x, camera.resolution_y);

    triangles_.resize(scene_data_->boxes.size() * 12);

    materials_.resize(scene_data_->boxes.size() * 12);

    for (size_t i = 0; i < scene_data_->boxes.size(); i++) {

      const auto& box = scene_data_->boxes[i];

      const auto& style = scene_data_->box_styles[box.style_index];

      const auto& size = scene_data_->box_styles[box.style_index];

      const auto x_vl = Vec3(size.box_size[0], 0, 0);
      const auto y_vl = Vec3(0, size.box_size[1], 0);
      const auto z_vl = Vec3(0, 0, size.box_size[2]);
      const auto zero = Vec3(0, 0, 0);

      // +X : right
      // -X : left
      // +Y : top
      // -Y : bottom
      // +Z : back
      // -Z : front

      const Texture* textures[12]{
        &style.front_color,
        &style.front_color,
        &style.back_color,
        &style.back_color,
        &style.top_color,
        &style.top_color,
        &style.bottom_color,
        &style.bottom_color,
        &style.left_color,
        &style.left_color,
        &style.right_color,
        &style.right_color,
      };

      const Tri triangles[12]{
        // front
        Tri(box.position - x_vl - y_vl - z_vl, box.position + x_vl - y_vl - z_vl, box.position + x_vl + y_vl - z_vl),
        Tri(box.position + x_vl + y_vl - z_vl, box.position - x_vl + y_vl - z_vl, box.position - x_vl - y_vl - z_vl),
        // back
        Tri(box.position + x_vl - y_vl + z_vl, box.position - x_vl - y_vl + z_vl, box.position - x_vl + y_vl + z_vl),
        Tri(box.position - x_vl + y_vl + z_vl, box.position + x_vl + y_vl + z_vl, box.position + x_vl - y_vl + z_vl),

        // top
        Tri(box.position - x_vl + y_vl - z_vl, box.position + x_vl + y_vl - z_vl, box.position + x_vl + y_vl + z_vl),
        Tri(box.position + x_vl + y_vl + z_vl, box.position - x_vl + y_vl + z_vl, box.position - x_vl + y_vl - z_vl),
        // bottom
        Tri(box.position + x_vl - y_vl + z_vl, box.position + x_vl - y_vl - z_vl, box.position - x_vl - y_vl - z_vl),
        Tri(box.position - x_vl - y_vl - z_vl, box.position - x_vl - y_vl + z_vl, box.position + x_vl - y_vl + z_vl),

        // left
        Tri(box.position - x_vl + y_vl - z_vl, box.position - x_vl + y_vl + z_vl, box.position - x_vl - y_vl + z_vl),
        Tri(box.position - x_vl - y_vl + z_vl, box.position - x_vl - y_vl - z_vl, box.position - x_vl + y_vl - z_vl),
        // right
        Tri(box.position + x_vl - y_vl + z_vl, box.position + x_vl + y_vl + z_vl, box.position + x_vl + y_vl - z_vl),
        Tri(box.position + x_vl + y_vl - z_vl, box.position + x_vl - y_vl - z_vl, box.position + x_vl - y_vl + z_vl),
      };

      for (size_t j = 0; j < 12; j++) {
        triangles_[(i * 12) + j] = triangles[j];
        materials_[(i * 12) + j] = Material{ textures[j], style.light_scale };
      }
    }

    std::vector<BoundingBox> bounding_boxes(triangles_.size());

    std::vector<Vec3> centers(triangles_.size());

    bvh::v2::ThreadPool thread_pool;

    bvh::v2::ParallelExecutor executor(thread_pool);

    executor.for_each(0,
                      triangles_.size(),
                      [&](const auto begin, const auto end) {
                        for (size_t i = begin; i < end; ++i) {
                          bounding_boxes[i] = triangles_[i].get_bbox();
                          centers[i] = triangles_[i].get_center();
                        }
                      });

    typename bvh::v2::DefaultBuilder<BvhNode>::Config config;

    config.quality = bvh::v2::DefaultBuilder<BvhNode>::Quality::High;

    bvh_ = bvh::v2::DefaultBuilder<BvhNode>::build(thread_pool, bounding_boxes, centers, config);
  }

  [[nodiscard]] std::vector<float> progress() const override
  {
    std::vector<float> result(jobs_.size());

    for (size_t i = 0; i < jobs_.size(); i++) {
      result[i] =
        static_cast<float>(jobs_[i].samples) /
        static_cast<float>(scene_data_->cameras[i].samples_per_pixel);
    }

    return result;
  }

  void iterate() override
  {
    for (size_t i = 0; i < jobs_.size(); i++)
      iterate_job(jobs_[i], i);
  }

  [[nodiscard]] bool done() const override
  {
    for (size_t i = 0; i < jobs_.size(); i++) {
      if (jobs_[i].samples != scene_data_->cameras[i].samples_per_pixel)
        return false;
    }

    return true;
  }

private:
  void iterate_job(Job& job, const size_t job_index) const
  {
    if (job.samples == scene_data_->cameras[job_index].samples_per_pixel)
      // Job is done, no need to render further.
      return;

    const Scalar x_scale = static_cast<Scalar>(1) / static_cast<Scalar>(job.width);
    const Scalar y_scale = static_cast<Scalar>(1) / static_cast<Scalar>(job.height);

    const Scalar aspect = static_cast<Scalar>(job.width) / static_cast<Scalar>(job.height);

    const Scalar sample_weight =
      static_cast<Scalar>(1) / scene_data_->cameras[job_index].samples_per_pixel;

    const auto& camera = scene_data_->cameras[job_index];

    const Scalar fov = std::tan(
      camera.field_of_view * static_cast<Scalar>(0.5) * static_cast<Scalar>(3.1415) * (1 / static_cast<Scalar>(180)));

    const auto ray_generator = camera.get_ray_generator();

#pragma omp parallel for

    for (int i = 0; i < static_cast<int>(job.color.size()); i++) {

      const auto x = static_cast<Scalar>(i % job.width);
      const auto y = static_cast<Scalar>(i / job.width);

      std::seed_seq seed{ i, job.samples };

      std::minstd_rand rng(seed);

      std::uniform_real_distribution<Scalar> dist(0, 1);

      const auto u = (x + dist(rng)) * x_scale;
      const auto v = (y + dist(rng)) * y_scale;

      const auto dx = ((u * 2) - 1) * fov * aspect;
      const auto dy = ((v * 2) - 1) * fov;

      const auto position = camera.position;

      const auto direction = ray_generator.generate(dx, dy);

      auto ray = Ray(position, direction);

      const auto c = trace(ray, rng);

      job.color[i][0] += c[0] * sample_weight;
      job.color[i][1] += c[1] * sample_weight;
      job.color[i][2] += c[2] * sample_weight;
    }

    job.samples++;

    if (job.samples == scene_data_->cameras[job_index].samples_per_pixel) {

      // Job has just completed. Tone map and save to output path.

      reinhard_tone_map(job.color);

      if (const Texture result(job.width, job.height, std::move(job.color)); !
        result.save_png(scene_data_->cameras[job_index].output_path.c_str())) {
        // TODO : report file save error
      }
    }
  }

  template<typename Rng>
  Vec3 trace(Ray& ray, Rng& rng, const int depth = 0) const
  {
    if (depth >= max_depth_)
      return { 0, 0, 0 };

    using Stack = bvh::v2::SmallStack<Bvh::Index, 64>;

    Stack stack;

    auto closest_index = std::numeric_limits<std::size_t>::max();

    Scalar u;
    Scalar v;

    bvh_.intersect<false, true>(ray,
                                bvh_.get_root().index,
                                stack,
                                [&](auto begin, auto end) {
                                  for (auto i = begin; i < end; ++i) {

                                    const auto id = bvh_.prim_ids[i];

                                    const auto hit = triangles_[id].intersect(ray);
                                    if (!hit)
                                      continue;

                                    std::tie(u, v) = *hit;

                                    closest_index = id;
                                  }

                                  return closest_index != std::numeric_limits<std::size_t>::max();
                                });

    if (closest_index == std::numeric_limits<std::size_t>::max())
      return { 0, 0, 0 };

    const auto& material = materials_[closest_index];

    const auto c = (closest_index % 2 == 0) ? material.texture->at(u, 1 - v) : material.texture->at(u, v);

    if (material.light_scale > 0)
      return Vec3(c[0], c[1], c[2]) * material.light_scale;

    const auto next_ray_org = ray.org + (ray.dir * (ray.tmax - shadow_bias_));

    const auto next_ray_dir = sample_hemisphere(normalize(triangles_[closest_index].n), rng);

    auto next_ray = Ray(next_ray_org, next_ray_dir);

    const auto indirect_light = trace(next_ray, rng, depth + 1);

    return { c[0] * indirect_light[0], c[1] * indirect_light[1], c[2] * indirect_light[2] };
  }

  template<typename Rng>
  static Vec3 sample_hemisphere(const Vec3& normal, Rng& rng)
  {
    std::uniform_real_distribution<Scalar> dist(-1, 1);
    while (true) {
      if (const Vec3 v(dist(rng), dist(rng), dist(rng)); dot(v, normal) >= 0)
        return normalize(v);
    }
  }

  static void reinhard_tone_map(std::vector<Texture::Color>& color)
  {
    const auto white = calculate_white(color);

    const auto white2 = Vec3(white[0] * white[0], white[1] * white[1], white[2] * white[2]);

    for (auto& c : color) {
      c[0] *= (1 + c[0] / white2[0]) / (1 + c[0]);
      c[1] *= (1 + c[1] / white2[1]) / (1 + c[1]);
      c[2] *= (1 + c[2] / white2[2]) / (1 + c[2]);
    }
  }

  static Vec3 calculate_white(const std::vector<Texture::Color>& color)
  {
    Vec3 max_white(0, 0, 0);

    for (const auto& c : color) {
      max_white[0] = max(max_white[0], c[0]);
      max_white[1] = max(max_white[1], c[1]);
      max_white[2] = max(max_white[2], c[2]);
    }

    return max_white;
  }

  const SceneData* scene_data_;

  std::vector<PrecomputedTri> triangles_;

  std::vector<Material> materials_;

  Bvh bvh_;

  std::vector<Job> jobs_;

  const int max_depth_ = 3;

  const Scalar shadow_bias_ = static_cast<Scalar>(1e-3);
};

} // namespace

std::unique_ptr<Renderer>
Renderer::create(const SceneData& scene)
{
  return std::make_unique<RendererImpl>(scene);
}

} // namespace mcr
