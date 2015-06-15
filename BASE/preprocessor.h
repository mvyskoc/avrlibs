/*
 * Copyright 2015 Martin Vyskocil <m.vyskoc@seznam.cz>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PREPROCESSOR_H_INCLUDED
#define PREPROCESSOR_H_INCLUDED

#define EMPTY()
#define EXPAND(...) __VA_ARGS__
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define GET_MACRO2(_1,_2, NAME,...) NAME
#define GET_MACRO8(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME
#define GET_MACRO9(_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME

#define ADD_ARGUMENT(...) _ADD_ARGUMENT(__VA_ARGS__)
#define _ADD_ARGUMENT(...) EMPTY(), ## __VA_ARGS__

/* If expr is not EMPTY return value */
#define IS_NOT_EMPTY(expr, value) _IS_NOT_EMPTY_BASE(value ADD_ARGUMENT(expr))
#define _IS_EMPTY_RETURN_VALUE(value, expr) value
#define _IS_EMPTY_RETURN_EMPTY(value)
#define _IS_NOT_EMPTY_BASE(...)                     \
    GET_MACRO2(__VA_ARGS__,                     \
               _IS_EMPTY_RETURN_VALUE,          \
               _IS_EMPTY_RETURN_EMPTY,          \
               )(__VA_ARGS__)

#endif // PREPROCESSOR_H_INCLUDED
