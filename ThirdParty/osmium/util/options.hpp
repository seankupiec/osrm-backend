#ifndef OSMIUM_UTIL_OPTIONS_HPP
#define OSMIUM_UTIL_OPTIONS_HPP

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

#include <initializer_list>
#include <map>
#include <string>
#include <utility>

namespace osmium {

    namespace util {

        /**
         * Stores key=value type options. This class can be used stand-alone or
         * as a base class. Options are stored and retrieved by key using the
         * different set() and get() methods.
         *
         * You can iterate over all set options. Dereferencing an iterator
         * yields a std::pair of the key and value strings.
         */
        class Options {

            typedef std::map<std::string, std::string> option_map;
            option_map m_options;

        public:

            typedef option_map::iterator iterator;
            typedef option_map::const_iterator const_iterator;
            typedef option_map::value_type value_type;

            Options() = default;

            explicit Options(std::initializer_list<value_type> values) :
                m_options(values) {
            }

            Options(const Options&) = default;
            Options& operator=(const Options&) = default;

            Options(Options&&) = default;
            Options& operator=(Options&&) = default;

            ~Options() = default;

            void set(const std::string& key, const std::string& value) {
                m_options[key] = value;
            }

            void set(const std::string& key, const char* value) {
                m_options[key] = value;
            }

            void set(const std::string& key, bool value) {
                m_options[key] = value ? "true" : "false";
            }

            void set(std::string data) {
                size_t pos = data.find_first_of('=');
                if (pos == std::string::npos) {
                    m_options[data] = "true";
                } else {
                    std::string value = data.substr(pos+1);
                    data.erase(pos);
                    set(data, value);
                }
            }

            /**
             * Get value of "key" option. If not set the default_value (or
             * empty string) is returned.
             */
            std::string get(const std::string& key, const std::string& default_value="") const noexcept {
                auto it = m_options.find(key);
                if (it == m_options.end()) {
                    return default_value;
                }
                return it->second;
            }

            /**
             * Is this option set to a true value ("true" or "yes")?
             */
            bool is_true(const std::string& key) const noexcept {
                std::string value = get(key);
                return (value == "true" || value == "yes");
            }

            size_t size() const noexcept {
                return m_options.size();
            }

            iterator begin() noexcept {
                return m_options.begin();
            }

            iterator end() noexcept {
                return m_options.end();
            }

            const_iterator begin() const noexcept {
                return m_options.cbegin();
            }

            const_iterator end() const noexcept {
                return m_options.cend();
            }

            const_iterator cbegin() const noexcept {
                return m_options.cbegin();
            }

            const_iterator cend() const noexcept {
                return m_options.cend();
            }

        }; // class Options

    } // namespace util

} // namespace osmium

#endif // OSMIUM_UTIL_OPTIONS_HPP
