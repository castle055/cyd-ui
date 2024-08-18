//
// Created by castle on 8/13/24.
//

export module cydui.std.charts.plot:series;

import std;
import fabric.linalg;

using la = with_precision<double>;

export namespace charts {
  class data_series_t {
    std::size_t data_len = 0;
    la::scalar *x_data = nullptr;
    la::scalar *y_data = nullptr;

  public:
    data_series_t() = default;

    template <std::size_t DATA_LEN>
    data_series_t(la::vec<DATA_LEN>& x, la::vec<DATA_LEN>& y)
    : data_len(DATA_LEN), x_data(x.data.data()), y_data(y.data.data()) { }

    std::size_t size() const {
      if (x_data == nullptr || y_data == nullptr) {
        return 0;
      }
      return data_len;
    }

    la::vec<2> operator[](std::size_t index) const {
      return {x_data[index], y_data[index]};
    }

    data_series_t& test() {
      return *this;
    }
  };
}