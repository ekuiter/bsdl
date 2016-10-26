#pragma once

#include <iostream>
#include <boost/range/adaptor/transformed.hpp>

using namespace std;

namespace {
    int levenshtein(const string& a, const string& b) {
        int m = a.length(), n = b.length();
        int d[m + 1][n + 1];

        d[0][0] = 0;
        for (int i = 1; i <= m; i++)
            d[i][0] = i;
        for (int j = 1; j <= n; j++)
            d[0][j] = j;

        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++)
                d[i][j] = min({
                        d[i - 1][j] + 1,
                        d[i][j - 1] + 1,
                        d[i - 1][j - 1] + (a[i] != b[j])
                });

        return d[m][n];
    }

    int smith_waterman(const string& a, const string& b, function<int (char, char, int)> pred = nullptr) {
        pred = pred ? pred : [](char a, char b, int gap) {
            return a == b ? 2 : -1;
        };

        int m = a.length(), n = b.length(), gap = -1, h_max = 0;
        int h[m + 1][n + 1];

        for (int i = 0; i <= m; i++)
            h[i][0] = 0;
        for (int j = 0; j <= n; j++)
            h[0][j] = 0;

        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++) {
                h[i][j] = max({
                        0,
                        h[i - 1][j - 1] + pred(a[i], b[j], gap),
                        h[i - 1][j] + pred(a[i], gap, gap),
                        h[i][j - 1] + pred(gap, b[j], gap)
                });
                h_max = max(h_max, h[i][j]);
            }

        return h_max;
    }
}

namespace util {
    double get_string_similarity(const string& a, const string& b) {
        return (double) smith_waterman(a, b) / (2 * max(a.length(), b.length()));
    }
}