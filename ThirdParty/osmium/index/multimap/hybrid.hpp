#ifndef OSMIUM_INDEX_MULTIMAP_HYBRID_HPP
#define OSMIUM_INDEX_MULTIMAP_HYBRID_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013,2014 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <cstddef>
#include <utility>

#include <osmium/index/multimap.hpp>
#include <osmium/index/multimap/stl_vector.hpp>
#include <osmium/index/multimap/stl_multimap.hpp>

namespace osmium {

    namespace index {

        namespace multimap {

            template <typename TId, typename TValue>
            class HybridIterator {

                typedef SparseMultimapMem<TId, TValue> main_map_type;
                typedef StlMultimap<TId, TValue> extra_map_type;

                typedef typename std::pair<TId, TValue> element_type;

                typename main_map_type::iterator m_begin_main;
                typename main_map_type::iterator m_end_main;
                typename extra_map_type::iterator m_begin_extra;
                typename extra_map_type::iterator m_end_extra;

            public:

                explicit HybridIterator(typename main_map_type::iterator begin_main,
                               typename main_map_type::iterator end_main,
                               typename extra_map_type::iterator begin_extra,
                               typename extra_map_type::iterator end_extra) :
                    m_begin_main(begin_main),
                    m_end_main(end_main),
                    m_begin_extra(begin_extra),
                    m_end_extra(end_extra) {
                }

                HybridIterator& operator++() {
                    if (m_begin_main == m_end_main) {
                        ++m_begin_extra;
                    } else {
                        ++m_begin_main;
                        while (m_begin_main != m_end_main && m_begin_main->second == osmium::index::empty_value<TValue>()) { // ignore removed elements
                            ++m_begin_main;
                        }
                    }
                    return *this;
                }

                HybridIterator<TId, TValue> operator++(int) {
                    auto tmp(*this);
                    operator++();
                    return tmp;
                }

                bool operator==(const HybridIterator& rhs) const {
                    return m_begin_main == rhs.m_begin_main &&
                           m_end_main   == rhs.m_end_main &&
                           m_begin_extra == rhs.m_begin_extra &&
                           m_end_extra   == rhs.m_end_extra;
                }

                bool operator!=(const HybridIterator& rhs) const {
                    return ! operator==(rhs);
                }

                const element_type& operator*() {
                    if (m_begin_main == m_end_main) {
                        return *m_begin_extra;
                    } else {
                        return *m_begin_main;
                    }
                }

                const element_type* operator->() {
                    return &operator*();
                }

            };

            template <typename TId, typename TValue>
            class Hybrid : public Multimap<TId, TValue> {

                typedef SparseMultimapMem<TId, TValue> main_map_type;
                typedef StlMultimap<TId, TValue> extra_map_type;

                main_map_type m_main;
                extra_map_type m_extra;

            public:

                typedef HybridIterator<TId, TValue> iterator;
                typedef const HybridIterator<TId, TValue> const_iterator;

                Hybrid() :
                    m_main(),
                    m_extra() {
                }

                size_t size() const override final {
                    return m_main.size() + m_extra.size();
                }

                size_t used_memory() const override final {
                    return m_main.used_memory() + m_extra.used_memory();
                }

                void reserve(const size_t size) {
                    m_main.reserve(size);
                }

                void unsorted_set(const TId id, const TValue value) {
                    m_main.set(id, value);
                }

                void set(const TId id, const TValue value) override final {
                    m_extra.set(id, value);
                }

                std::pair<iterator, iterator> get_all(const TId id) {
                    auto result_main = m_main.get_all(id);
                    auto result_extra = m_extra.get_all(id);
                    return std::make_pair(iterator(result_main.first, result_main.second, result_extra.first, result_extra.second),
                                          iterator(result_main.second, result_main.second, result_extra.second, result_extra.second));
                }

                void remove(const TId id, const TValue value) {
                    m_main.remove(id, value);
                    m_extra.remove(id, value);
                }

                void consolidate() {
                    m_main.erase_removed();
                    for (const auto& element : m_extra) {
                        m_main.set(element.first, element.second);
                    }
                    m_extra.clear();
                    m_main.sort();
                }

                void dump_as_list(int fd) {
                    consolidate();
                    m_main.dump_as_list(fd);
                }

                void clear() override final {
                    m_main.clear();
                    m_extra.clear();
                }

                void sort() override final {
                    m_main.sort();
                }

            }; // Hybrid

        } // namespace multimap

    } // namespace index

} // namespace osmium

#endif // OSMIUM_INDEX_MULTIMAP_HYBRID_HPP
