#include "Renderer.h"

#include "Scene.h"

namespace mcr {

namespace {

struct Job final
{
  Texture color;

  int samples{ 0 };

  Job(int w, int h)
    : color(w, h, std::vector<Texture::Color>(w * h))
  {
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
  }

  std::vector<float> progress() const override
  {
    std::vector<float> result;
    return result;
  }

  void iterate() override
  {
    for (size_t i = 0; i < jobs_.size(); i++)
      iterate_job(jobs_[i], i);
  }

  bool done() const override
  {
    for (size_t i = 0; i < jobs_.size(); i++) {
      if (jobs_[i].samples != scene_data_->cameras[i].samples_per_pixel)
        return false;
    }

    return true;
  }

private:
  void iterate_job(Job& job, size_t job_index)
  {
    if (job.samples == job.color.pixel_count())
      // Job is done, no need to render further.
      return;

    for (int i = 0; i < job.color.pixel_count(); i++) {
    }

    job.samples++;

    if (job.samples == job.color.pixel_count()) {

      // Job has just completed. Save to output path.

      job.color.save_png(scene_data_->cameras[job_index].output_path.c_str());
    }
  }

private:
  const SceneData* scene_data_;

  std::vector<Job> jobs_;
};

} // namespace

std::unique_ptr<Renderer>
Renderer::create(const SceneData& scene)
{
  return std::make_unique<RendererImpl>(scene);
}

} // namespace mcr
