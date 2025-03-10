#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../include/extract_mkv_k4a.h"


namespace extract_mkv {

  const int MAX_PARALLEL_JOBS = 12;

  class Semaphore {
  public:
      Semaphore (int count_ = 0)
          : count(count_) {}

      inline void notify()
      {
          std::unique_lock<std::mutex> lock(mtx);
          count++;
          cv.notify_one();
      }

      inline void wait()
      {
          std::unique_lock<std::mutex> lock(mtx);

          while(count == 0){
              cv.wait(lock);
          }
          count--;
      }

  private:
      std::mutex mtx;
      std::condition_variable cv;
      int count;
  };

  class Timesynchronizer { 
    public: explicit Timesynchronizer(const size_t, const size_t, 
            const size_t, ExportConfig, const bool, const bool);
      void initialize_feeds(std::vector<fs::path>, fs::path);
      void feed_forward(int);
      void run();
      void monitor();

    protected:
      std::vector<std::shared_ptr<K4AFrameExtractor>> m_input_feeds;

      const size_t m_first_frame{0};
      const size_t m_last_frame{0};
      const size_t m_skip_frames{1};
      // kinect devices work in microseconds
      std::chrono::microseconds m_sync_window{0};

      bool m_use_timesync;
      bool m_enable_seek;
      ExportConfig m_export_config;

      Semaphore m_sem{MAX_PARALLEL_JOBS};
      std::mutex m_lock;
      std::mutex m_thread_free_lock;
      std::condition_variable m_wait_cv;
      std::vector<std::thread> m_worker_threads;
      std::vector<std::thread::id> m_finished_threads;

      std::thread m_monitor_thread;
      bool m_is_running{false};
  };

}
