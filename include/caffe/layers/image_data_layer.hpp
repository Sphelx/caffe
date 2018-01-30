#ifndef CAFFE_IMAGE_DATA_LAYER_HPP_
#define CAFFE_IMAGE_DATA_LAYER_HPP_

#include <string>
#include <utility>
#include <vector>

#include "caffe/blob.hpp"
#include "caffe/data_transformer.hpp"
#include "caffe/internal_thread.hpp"
#include "caffe/layer.hpp"
#include "caffe/layers/base_data_layer.hpp"
#include "caffe/proto/caffe.pb.h"

namespace caffe {

/**
 * @brief Provides data to the Net from image files.
 *
 * TODO(dox): thorough documentation for Forward and proto params.
 */
template <typename Ftype, typename Btype>
class ImageDataLayer : public BasePrefetchingDataLayer<Ftype, Btype> {
 public:
  ImageDataLayer(const LayerParameter& param, size_t solver_rank);
  virtual ~ImageDataLayer();
  void DataLayerSetUp(const vector<Blob*>& bottom, const vector<Blob*>& top) override;

  bool ShareInParallel() const override {
    return false;
  }
  const char* type() const override {
    return "ImageData";
  }
  int ExactNumBottomBlobs() const override {
    return 0;
  }
  int ExactNumTopBlobs() const override {
    return 2;
  }

 protected:
  void ShuffleImages();
  void load_batch(Batch* batch, int thread_id, size_t queue_id = 0UL) override;
  void start_reading() override {}
  void InitializePrefetch() override;
  bool auto_mode() const override {
    return false;
  }

  Flag* layer_inititialized_flag() override {
    return this->phase_ == TRAIN ? &layer_inititialized_flag_ : nullptr;
  }

  shared_ptr<Caffe::RNG> prefetch_rng_;
  Flag layer_inititialized_flag_;
  size_t epoch_count_;
  vector<size_t> line_ids_;
  static vector<vector<std::pair<std::string, int>>> lines_;  // per phase
  static vector<unordered_map<std::string, cv::Mat>> cache_;
  static vector<bool> cached_;
  static vector<size_t> cached_num_, failed_num_;
  static vector<std::mutex> cache_mutex_;
};

template <typename Ftype, typename Btype>
vector<vector<std::pair<std::string, int>>> ImageDataLayer<Ftype, Btype>::lines_(Phase_ARRAYSIZE);
template <typename Ftype, typename Btype>
vector<unordered_map<std::string, cv::Mat>> ImageDataLayer<Ftype, Btype>::cache_(Phase_ARRAYSIZE);
template <typename Ftype, typename Btype>
vector<bool> ImageDataLayer<Ftype, Btype>::cached_(Phase_ARRAYSIZE);
template <typename Ftype, typename Btype>
vector<size_t> ImageDataLayer<Ftype, Btype>::cached_num_(Phase_ARRAYSIZE);
template <typename Ftype, typename Btype>
vector<size_t> ImageDataLayer<Ftype, Btype>::failed_num_(Phase_ARRAYSIZE);
template <typename Ftype, typename Btype>
vector<std::mutex> ImageDataLayer<Ftype, Btype>::cache_mutex_(Phase_ARRAYSIZE);

}  // namespace caffe

#endif  // CAFFE_IMAGE_DATA_LAYER_HPP_
