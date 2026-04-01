// CSR Matrix implementation for Problem 039
#ifndef CSR_MATRIX_HPP
#define CSR_MATRIX_HPP

#include <vector>
#include <exception>

namespace sjtu {

class size_mismatch : public std::exception {
public:
    const char *what() const noexcept override {
        return "Size mismatch";
    }
};

class invalid_index : public std::exception {
public:
    const char *what() const noexcept override {
        return "Index out of range";
    }
};

template <typename T>
class CSRMatrix {

private:
    size_t n_rows{};
    size_t n_cols{};
    size_t nnz{};
    std::vector<size_t> indptr;   // size n_rows+1
    std::vector<size_t> indices;  // size nnz
    std::vector<T> data;          // size nnz
    // binary search within row range [l, r) for column j
    size_t lower_bound_pos(size_t l, size_t r, size_t j) const {
        while (l < r) {
            size_t mid = l + ((r - l) >> 1);
            if (indices[mid] < j) l = mid + 1; else r = mid;
        }
        return l;
    }

public:
    CSRMatrix &operator=(const CSRMatrix &other) = delete;
    CSRMatrix &operator=(CSRMatrix &&other) = delete;

    CSRMatrix(size_t n, size_t m)
        : n_rows(n), n_cols(m), nnz(0), indptr(n + 1, 0) {}

    CSRMatrix(size_t n, size_t m, size_t count,
        const std::vector<size_t> &indptr_,
        const std::vector<size_t> &indices_,
        const std::vector<T> &data_)
        : n_rows(n), n_cols(m), nnz(count), indptr(indptr_), indices(indices_), data(data_)
    {
        if (indptr.size() != n_rows + 1) throw size_mismatch();
        if (indices.size() != nnz || data.size() != nnz) throw size_mismatch();
        if (!indptr.empty() && indptr.front() != 0) throw size_mismatch();
        if (!indptr.empty() && indptr.back() != nnz) throw size_mismatch();
        for (size_t i = 0; i + 1 < indptr.size(); ++i) if (indptr[i] > indptr[i + 1]) throw size_mismatch();
        for (size_t c : indices) if (c >= n_cols) throw size_mismatch();
    }

    CSRMatrix(const CSRMatrix &other) = default;
    CSRMatrix(CSRMatrix &&other) = default;

    CSRMatrix(size_t n, size_t m, const std::vector<std::vector<T>> &dense)
        : n_rows(n), n_cols(m), nnz(0), indptr(n + 1, 0)
    {
        if (dense.size() != n_rows) throw size_mismatch();
        for (size_t i = 0; i < n_rows; ++i) {
            if (dense[i].size() != n_cols) throw size_mismatch();
            for (size_t j = 0; j < n_cols; ++j) {
                if (dense[i][j] != T()) {
                    indices.push_back(j);
                    data.push_back(dense[i][j]);
                    ++nnz;
                }
            }
            indptr[i + 1] = nnz;
        }
    }

    ~CSRMatrix() = default;

    size_t getRowSize() const { return n_rows; }
    size_t getColSize() const { return n_cols; }
    size_t getNonZeroCount() const { return nnz; }

    T get(size_t i, size_t j) const {
        if (i >= n_rows || j >= n_cols) throw invalid_index();
        size_t l = indptr[i];
        size_t r = indptr[i + 1];
        size_t pos = lower_bound_pos(l, r, j);
        if (pos < r && indices[pos] == j) return data[pos];
        return T();
    }

    void set(size_t i, size_t j, const T &value) {
        if (i >= n_rows || j >= n_cols) throw invalid_index();
        size_t l = indptr[i];
        size_t r = indptr[i + 1];
        size_t pos = lower_bound_pos(l, r, j);
        if (pos < r && indices[pos] == j) {
            data[pos] = value;
            return;
        }
        // insert new entry at position pos to keep columns sorted (no delete required by spec)
        indices.insert(indices.begin() + pos, j);
        data.insert(data.begin() + pos, value);
        ++nnz;
        for (size_t k = i + 1; k < indptr.size(); ++k) ++indptr[k];
    }

    const std::vector<size_t> &getIndptr() const { return indptr; }
    const std::vector<size_t> &getIndices() const { return indices; }
    const std::vector<T> &getData() const { return data; }

    std::vector<std::vector<T>> getMatrix() const {
        std::vector<std::vector<T>> dense(n_rows, std::vector<T>(n_cols, T()));
        for (size_t i = 0; i < n_rows; ++i) {
            for (size_t p = indptr[i]; p < indptr[i + 1]; ++p) {
                dense[i][indices[p]] = data[p];
            }
        }
        return dense;
    }

    std::vector<T> operator*(const std::vector<T> &vec) const {
        if (vec.size() != n_cols) throw size_mismatch();
        std::vector<T> res(n_rows, T());
        for (size_t i = 0; i < n_rows; ++i) {
            T acc = T();
            for (size_t p = indptr[i]; p < indptr[i + 1]; ++p) {
                acc = acc + data[p] * vec[indices[p]];
            }
            res[i] = acc;
        }
        return res;
    }

    CSRMatrix getRowSlice(size_t l, size_t r) const {
        if (l > r || r > n_rows) throw invalid_index();
        size_t rows = r - l;
        CSRMatrix sub(rows, n_cols);
        sub.nnz = indptr[r] - indptr[l];
        sub.indptr.assign(rows + 1, 0);
        sub.indices.resize(sub.nnz);
        sub.data.resize(sub.nnz);
        for (size_t i = 0; i < rows; ++i) {
            size_t start = indptr[l + i];
            size_t end = indptr[l + i + 1];
            size_t len = end - start;
            sub.indptr[i + 1] = sub.indptr[i] + len;
            for (size_t p = 0; p < len; ++p) {
                sub.indices[sub.indptr[i] + p] = indices[start + p];
                sub.data[sub.indptr[i] + p] = data[start + p];
            }
        }
        return sub;
    }
};

}

#endif // CSR_MATRIX_HPP
