#pragma once

#include <mm/components/position2d.hpp>
#include <mm/components/position2d_zoffset.hpp>
#include <mm/components/position3d.hpp>

#include <mm/components/scale2d.hpp>
#include <mm/components/rotation2d.hpp>

#include <mm/components/transform4x4.hpp>

#include <entt/entity/registry.hpp>

namespace MM::Systems {

// 2D usage:
//
// auto create dependencies, eg:
// scene.on_construct<Components::Position2D>().connect<&entt::registry::emplace_or_replace<Components::Position3D>>();
// scene.on_construct<Components::Position2D>().connect<&entt::registry::emplace_or_replace<Components::Transform4x4>>();
// scene.on_construct<Components::Position2D>().connect<&entt::registry::emplace_or_replace<Components::DirtyTransformTag>>();
//
// mark as dirty on change, eg:
// scene.on_update<Components::Position2D>().connect<&entt::registry::emplace_or_replace<Components::DirtyTransformTag>>();
// scene.on_update<Components::Position2D_ZOffset>().connect<&entt::registry::emplace_or_replace<Components::DirtyTransformTag>>();
// scene.on_update<Components::Position3D>().connect<&entt::registry::emplace_or_replace<Components::DirtyTransformTag>>();
// scene.on_update<Components::Scale2D>().connect<&entt::registry::emplace_or_replace<Components::DirtyTransformTag>>();
// scene.on_update<Components::Rotation2D>().connect<&entt::registry::emplace_or_replace<Components::DirtyTransformTag>>();
//
// optinally cleanup on destruction, eg:
// scene.on_destroy<Components::Position2D>().connect<&entt::registry::remove<
//     Components::DirtyTransformTag,
//     Components::Position2D_ZOffset,
//     Components::Position3D,
//     Components::Scale2D,
//     Components::Rotation2D,
//     Components::Transform4x4
// >>();


// TODO: impl for no zoffset comp
void position3d_from_2d(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Position2D, const Components::Position2D_ZOffset, Components::Position3D>> view);

void transform3d_translate(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Position3D, Components::Transform4x4>> view);

void transform3d_rotate2d(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Rotation2D, Components::Transform4x4>> view);

void transform3d_scale2d(entt::view<entt::get_t<const Components::DirtyTransformTag, const Components::Scale2D, Components::Transform4x4>> view);

void transform_clear_dirty(entt::registry& scene, entt::view<entt::get_t<Components::DirtyTransformTag>> view);

} // MM::Systems

