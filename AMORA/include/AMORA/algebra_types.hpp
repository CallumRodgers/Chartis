////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_MAVISTYPES_HPP
#define CHARTIS_MAVISTYPES_HPP

#include <glm/matrix.hpp>

#ifdef DOUBLE_PRECISION
typedef double m_float;
typedef glm::dvec2 m_vec2;
typedef glm::dvec3 m_vec3;
typedef glm::dvec4 m_vec4;
typedef glm::dmat2x2 m_mat2x2;
typedef glm::dmat3x3 m_mat3x3;
typedef glm::dmat4x4 m_mat4x4;
#else
typedef float m_float;
typedef glm::vec2 m_vec2;
typedef glm::vec3 m_vec3;
typedef glm::vec4 m_vec4;
typedef glm::mat2x2 m_mat2x2;
typedef glm::mat3x3 m_mat3x3;
typedef glm::mat4x4 m_mat4x4;
#endif

#endif //CHARTIS_MAVISTYPES_HPP
