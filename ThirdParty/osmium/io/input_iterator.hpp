#ifndef OSMIUM_IO_INPUT_ITERATOR_HPP
#define OSMIUM_IO_INPUT_ITERATOR_HPP

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

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

#include <osmium/memory/buffer.hpp>
#include <osmium/memory/item.hpp>

namespace osmium {

    namespace io {

        /**
         * This iterator class allows you to iterate over all items from a
         * source. It hides all the buffer handling and makes the contents of a
         * source accessible as a normal STL input iterator.
         */
        template <class TSource, class TItem = osmium::memory::Item>
        class InputIterator {

            static_assert(std::is_base_of<osmium::memory::Item, TItem>::value, "TItem must derive from osmium::buffer::Item");

            typedef typename osmium::memory::Buffer::t_iterator<TItem> item_iterator;

            TSource* m_source;
            std::shared_ptr<osmium::memory::Buffer> m_buffer {};
            item_iterator m_iter {};

            void update_buffer() {
                do {
                    m_buffer = std::make_shared<osmium::memory::Buffer>(std::move(m_source->read()));
                    if (!m_buffer || !*m_buffer) { // end of input
                        m_source = nullptr;
                        m_buffer.reset();
                        m_iter = item_iterator();
                        return;
                    }
                    m_iter = m_buffer->begin<TItem>();
                } while (m_iter == m_buffer->end<TItem>());
            }

        public:

            typedef std::input_iterator_tag iterator_category;
            typedef TItem                   value_type;
            typedef ptrdiff_t               difference_type;
            typedef TItem*                  pointer;
            typedef TItem&                  reference;

            explicit InputIterator(TSource& source) :
                m_source(&source) {
                update_buffer();
            }

            // end iterator
            InputIterator() :
                m_source(nullptr) {
            }

            InputIterator& operator++() {
                assert(m_source);
                assert(m_buffer);
                assert(m_iter);
                ++m_iter;
                if (m_iter == m_buffer->end<TItem>()) {
                    update_buffer();
                }
                return *this;
            }

            InputIterator operator++(int) {
                InputIterator tmp(*this);
                operator++();
                return tmp;
            }

            bool operator==(const InputIterator& rhs) const {
                return m_source == rhs.m_source &&
                       m_buffer == rhs.m_buffer &&
                       m_iter == rhs.m_iter;
            }

            bool operator!=(const InputIterator& rhs) const {
                return !(*this == rhs);
            }

            reference operator*() const {
                assert(m_iter);
                return static_cast<reference>(*m_iter);
            }

            pointer operator->() const {
                assert(m_iter);
                return &static_cast<reference>(*m_iter);
            }

        }; // class InputIterator

    } // namespace io

} // namespace osmium

#endif // OSMIUM_IO_INPUT_ITERATOR_HPP
