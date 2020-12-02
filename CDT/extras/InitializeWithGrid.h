/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/**
 * @file
 * Helper function to initialize triangulation with regular grid instead of
 * super-triangle
 */

#ifndef CDT_HmXGv083vZCrT3OXASD9
#define CDT_HmXGv083vZCrT3OXASD9

#include "CDT.h"
#include "CDTUtils.h"

#include <cstddef>
#include <iterator>

namespace CDT
{
namespace detail
{

template <typename T, typename OutputIt>
void generateVertices(
    OutputIt outFirst,
    const T xmin,
    const T xmax,
    const T ymin,
    const T ymax,
    const std::size_t xres,
    const std::size_t yres)
{
    const T xstep = (xmax - xmin) / xres;
    const T ystep = (ymax - ymin) / yres;
    T y = ymin;
    for(std::size_t iy = 0; iy <= yres; ++iy, y += ystep)
    {
        T x = xmin;
        for(std::size_t ix = 0; ix <= xres; ++ix, x += xstep)
        {
            Vertex<T> v;
            v.pos = V2d<T>::make(x, y);

            const std::size_t i = iy * xres + ix;
            // left-up
            if(ix > 0 && iy > 0)
            {
                v.triangles.push_back(2 * (i - xres - 1));
                v.triangles.push_back(2 * (i - xres - 1) + 1);
            }
            // right-up
            if(ix < xres && iy > 0)
            {
                 v.triangles.push_back(2 * (i - xres));
            }
            // left-down
            if(ix > 0 && iy < yres)
            {
                v.triangles.push_back(2 * (i - 1) + 1);
            }
            // right-down
            if(ix < xres && iy < yres)
            {
                v.triangles.push_back(2 * i);
                v.triangles.push_back(2 * i + 1);
            }
            *outFirst++ = v;
        }
    }
}

template <typename OutputIt>
void generateTriangles(
    OutputIt outFirst,
    const std::size_t xres,
    const std::size_t yres)
{
    std::size_t i = 0;
    for(std::size_t iy = 0; iy < yres; ++iy)
    {
        for(std::size_t ix = 0; ix < xres; ++ix, i+=2)
        {
            // 0___1           v3
            // |\  |           /\
            // | \ |        n3/  \n2
            // |__\|         /____\
            // 2   3       v1  n1  v2
            const VertInd vv[4] = {i, i + 1, i + xres, i + xres + 1};
            Triangle t;

            t.vertices = {vv[0], vv[2], vv[3]};
            t.neighbors = {
                ix ? i - 1 : noNeighbor,
                iy < yres - 1 ? i + xres * 2 + 1 : noNeighbor,
                i + 1};
            *outFirst++ = t;

            t.vertices = {vv[0], vv[3], vv[1]};
            t.neighbors = {
                i,
                ix < xres - 1 ? i + 2 : noNeighbor,
                iy ? i - xres * 2 : noNeighbor};
            *outFirst++ = t;
        }
    }
}

} // namespace detail

/**
 * Make a triangulation that uses regular grid triangles instead of
 * super-triangle
 *
 * @tparam T type of vertex coordinates (e.g., float, double)
 */
template <typename T>
void initializeWithGrid(
    const T xmin,
    const T xmax,
    const T ymin,
    const T ymax,
    const std::size_t xres,
    const std::size_t yres,
    Triangulation<T>& out)
{
    out.triangles.reserve(xres * yres * 2);
    out.vertices.reserve((xres + 1) * (yres + 1));
    detail::generateVertices(
        std::back_inserter(out.vertices), xmin, xmax, ymin, ymax, xres, yres);
    detail::generateTriangles(std::back_inserter(out.triangles), xres, yres);
}

} // namespace CDT

#endif
