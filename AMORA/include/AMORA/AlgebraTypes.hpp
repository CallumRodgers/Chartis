////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_MAVISTYPES_HPP
#define OPENMAVIS_MAVISTYPES_HPP

#include <glm/matrix.hpp>

#ifdef DOUBLE_PRECISION
typedef double m_float;
typedef glm::dvec2 m_vec2;
typedef glm::dvec3 m_vec3;
typedef glm::dvec4 m_vec4;
typedef glm::dmat4x4 m_mat4x4;
#else
typedef float m_float;
typedef glm::vec2 m_vec2;
typedef glm::vec3 m_vec3;
typedef glm::vec4 m_vec4;
typedef glm::mat4x4 m_mat4x4;
#endif

#endif //OPENMAVIS_MAVISTYPES_HPP
