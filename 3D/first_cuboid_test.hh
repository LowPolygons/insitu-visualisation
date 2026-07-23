#ifndef FIRST_CUBOID_TEST_HH
#define FIRST_CUBOID_TEST_HH

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>
namespace Cuboid {
/*
 * Given the three slices dimensions, it needs to calculate the positions in 3D
 * space.
 *
 * This can just be the dimensions relative to 0,0
 *
 * Set the camera position to a fixed point (ratio-wise)
 * Calculate the longest length and the height and move it that far up from the
 * top, and that far lengthwise away
 *
 * Can probably mathematically calculate the angle it should look
 *
 * Using that camera position and direction calculate the projection plane and
 * do the standard per pixel operation, defaulting to a colour
 * -> One change is that as soon as a surface is found, it can break from the
 * loop over the objects
 * -> Generally speaking, the first time an object is found to be intersected,
 * it should be moved to the front of the array as it will be colided with more
 *
 * Potential improvements: mathematically calculate the positions of the CORNERS
 * of each face
 * -> do these need to be relative to the projection plane or in broad 3d space?
 * Then calculate a step size in each axis and do a loop to calculate the colour
 * Theoretically reduces all computation to be only on the pixels that will have
 * a mesh
 *
 * Do that last
 *
 * Future optimisations:
 *
 * the computation of where the cuboid is should in theory be constant - perform
 * it once, return some sort of structure and every iteration just map the
 * mashes
 */

//==// Vec3 Utility
template <typename T> using Vec3 = std::array<T, 3>;

template <typename T>
constexpr auto operator+(const Vec3<T> &a, const Vec3<T> &b) -> Vec3<T> {
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}
template <typename T>
constexpr auto operator-(const Vec3<T> &a, const Vec3<T> &b) -> Vec3<T> {
  return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}
template <typename T>
constexpr auto operator*(const Vec3<T> &a, const Vec3<T> &b) -> Vec3<T> {
  return {a[0] * b[0], a[1] * b[1], a[2] * b[2]};
}
// NOTE: this is a vec + scalar overload
template <typename T>
constexpr auto operator*(T b, const Vec3<T> &a) -> Vec3<T> {
  return {a[0] * b, a[1] * b, a[2] * b};
}
template <typename T>
constexpr auto operator/(const Vec3<T> &a, const Vec3<T> &b) -> Vec3<T> {
  return {a[0] / b[0], a[1] / b[1], a[2] / b[2]};
}
template <typename T> auto stringVec(const Vec3<T> &v) -> std::string {
  return "[" + std::to_string(v[0]) + ", " + std::to_string(v[1]) + ", " +
         std::to_string(v[2]) + "]";
}

/*
 * WARN: Potential subject for change
 *
 * In the workflow, 3 of these are created
 */
template <typename T> struct Face {
  Vec3<T> origin;
  Vec3<T> a;
  Vec3<T> b;
};

template <typename T> auto printFace(const Face<T> &face) -> void {
  std::cout << "Face: " << std::endl;
  std::cout << "- Origin: " << stringVec(face.origin) << std::endl;
  std::cout << "- Dir Vec A: " << stringVec(face.a) << std::endl;
  std::cout << "- Dir Vec B: " << stringVec(face.b) << std::endl;
}

template <typename T> struct Camera {
  Vec3<T> origin;
  Vec3<T> direction;
};

template <typename T>
auto get_faces_of_cuboid(const std::array<std::size_t, 3> &dims)
    -> std::array<Face<T>, 3> {
  // The cuboid has its front left bottom corner on 0,0,0 and extends right in
  // the X, up the in the Y and forward in the Z
  //
  // The direction vectors for the face should only have one non-zero
  // coefficient

  return {
      Face<T>{{T{0}, T{0}, T{0}}, {T{0}, dims[1], T{0}}, {dims[0], T{0}, T{0}}},
      Face<T>{
          {dims[0], T{0}, T{0}}, {T{0}, dims[1], T{0}}, {T{0}, T{0}, dims[2]}},
      Face<T>{ // WARN: May need to swap the first two lines
              {/* faces[2].second */
               T{0}, dims[1], T{0}},
              {T{0}, T{0}, dims[2]},
              {/* T{-1} * */ dims[0], T{0}, T{0}}}};
}

// /*
//  * WARN: An assumption has to be made here about the order of the faces and
//  how
//  * theyve been inserted
//  * - The first face will be parallel with the XY plane (vert lying)
//  * - The second face will be parallel with the YZ plane (vert lying)
//  * - The third will be parallel with the XZ plane (horiz lying)
//  */
// template <typename T>
// auto get_faces_of_cuboid(
//     const std::array<std::pair<std::size_t, std::size_t>, 3> &faces)
//     -> std::array<Face<T>, 3> {
//   // The cuboid has its front left bottom corner on 0,0,0 and extends right
//   in
//   // the X, up the in the Y and forward in the Z
//   //
//   // The direction vectors for the face should only have one non-zero
//   // coefficient
//
//   return {Face<T>{{T{0}, T{0}, T{0}},
//                   {faces[0].first, T{0}, T{0}},
//                   {T{0}, faces[0].second, T{0}}},
//           Face<T>{{faces[0].first, T{0}, T{0}},
//                   {T{0}, faces[1].second, T{0}},
//                   {T{0}, T{0}, faces[1].first}},
//           Face<T>{ // WARN: May need to swap the first two lines
//                   {/* faces[2].second */
//                    T{0}, faces[0].second, T{0}},
//                   {T{0}, T{0}, faces[2].first},
//                   {/* T{-1} * */ faces[2].second, T{0}, T{0}}}};
// }

template <typename T>
auto get_position_camera(const std::array<std::size_t, 3> &dims) -> Camera<T> {
  auto max_hoz_distance = std::max(dims[0], dims[2]);

  auto distance_multipler = 0.8;
  auto camera_pos =
      distance_multipler * Vec3<T>{2 * static_cast<T>(max_hoz_distance),
                                   1.75 * static_cast<T>(dims[1]),
                                   -1 * static_cast<T>(max_hoz_distance)};

  // INFO: try to set the direction to be the center of the thing
  auto center_of_cuboid = Vec3<T>{
      dims[0] / T{2},
      dims[1] / T{2},
      dims[2] / T{2},
  };

  std::cout << "CENTER OF CUBOID: " << stringVec(center_of_cuboid) << std::endl;

  auto camera_angle = center_of_cuboid - camera_pos;

  auto mod = std::sqrt(camera_angle[0] * camera_angle[0] +
                       camera_angle[1] * camera_angle[1] +
                       camera_angle[2] * camera_angle[2]);

  camera_angle = {camera_angle[0] / mod, -1 * camera_angle[1] / mod,
                  camera_angle[2] / mod};

  return Camera<T>{camera_pos, camera_angle};
}

template <typename T>
auto get_position_camera(const std::array<Face<T>, 3> &faces) -> Camera<T> {
  auto max_hoz_distance = std::max(faces[0].a[0], faces[1].a[2]);
  auto shorter_hoz_distance = std::min(faces[0].a[0], faces[1].a[2]);

  auto distance_multipler = 0.8;
  auto camera_pos =
      distance_multipler * Vec3<T>{2 * max_hoz_distance, 1.75 * faces[0].b[1],
                                   -1 * max_hoz_distance};

  // INFO: try to set the direction to be the center of the thing
  auto center_of_cuboid = Vec3<T>{
      faces[0].a[0] / T{2},
      faces[0].b[1] / T{2},
      faces[1].b[2] / T{2},
  };

  std::cout << "CENTER OF CUBOID: " << stringVec(center_of_cuboid) << std::endl;

  auto camera_angle = center_of_cuboid - camera_pos;

  auto mod = std::sqrt(camera_angle[0] * camera_angle[0] +
                       camera_angle[1] * camera_angle[1] +
                       camera_angle[2] * camera_angle[2]);

  camera_angle = {camera_angle[0] / mod, -1 * camera_angle[1] / mod,
                  camera_angle[2] / mod};

  return Camera<T>{camera_pos, camera_angle};
}

template <typename T>
auto get_pixel_buffer(
    std::pair<std::size_t, std::size_t> image_dimensions, Camera<T> camera,
    std::array<Face<T>, 3> faces,
    const std::array<std::vector<std::uint8_t>, 3> &image_buffers,
    const std::array<std::pair<std::size_t, std::size_t>, 3> &image_buffer_dims)
    -> std::vector<std::uint8_t> {
  /*
   * Initialise the pixel buffer
   */
  auto pixel_buffer = std::vector<std::uint8_t>{};

  /*
   *
   * Camera nonesense (necessary)
   *
   */
  constexpr auto field_of_view = 60 * 3.1415926535 / 180;
  auto aspect_ratio = image_dimensions.first / image_dimensions.second;

  auto projection_plane_w = tan(field_of_view / 2) * 2;
  auto projection_plane_h = projection_plane_w / aspect_ratio;

  auto proj_plane_camera_offset_x = projection_plane_w / 2;
  auto proj_plane_camera_offset_y = projection_plane_h / 2;

  auto pixel_increment_x =
      projection_plane_w / static_cast<double>(image_dimensions.first);
  auto pixel_increment_y =
      projection_plane_h / static_cast<double>(image_dimensions.second);

  // WARN: making the assumption the direction vector is normalised, and the
  // forward vector is (0,0,1)
  auto camera_yaw = std::atan2(camera.direction[0], camera.direction[2]);
  auto camera_pitch =
      std::atan2(camera.direction[1],
                 std::sqrt(camera.direction[0] * camera.direction[0] +
                           camera.direction[2] * camera.direction[2]));

  /*
   *
   * Helper method for getting normal and d of a face as a plane
   *
   */
  auto calculate_normal_and_d_of_face =
      [](const Face<T> &face) -> std::pair<Vec3<T>, T> {
    auto v = Vec3<T>{face.a[1] * face.b[2] - face.b[1] * face.a[2],
                     -1 * face.a[0] * face.b[2] + face.b[0] * face.a[2],
                     face.a[0] * face.b[1] - face.b[0] * face.a[1]};

    auto v_mod = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v = {v[0] / v_mod, v[1] / v_mod, v[2] / v_mod};

    return {v, face.origin[0] * v[0] + face.origin[1] * v[1] +
                   face.origin[2] * v[2]};
  };

  auto pre_calculated_face_normals_and_ds =
      std::array<std::pair<Vec3<T>, T>, 3>{
          {calculate_normal_and_d_of_face(faces[0]),
           calculate_normal_and_d_of_face(faces[1]),
           calculate_normal_and_d_of_face(faces[2])}};

  std::cout << "Image actual dimensions" << std::endl;
  for (auto thing : image_buffer_dims) {
    std::cout << thing.first << " x " << thing.second << std::endl;
  }
  /*
   *
   * Main computation
   *
   */
  for (int pixel_y = 0; pixel_y < image_dimensions.second; pixel_y++) {
    for (int pixel_x = 0; pixel_x < image_dimensions.second; pixel_x++) {
      // Calculate the ray for the current pixel

      // Point on projection plane
      auto proj_plane_point = Vec3<double>{
          (pixel_increment_x * pixel_x) - proj_plane_camera_offset_x,
          (pixel_increment_y * pixel_y) - proj_plane_camera_offset_y, 0.0};

      auto dir_vector = proj_plane_point - Vec3<T>{T{0}, T{0}, T{-1}};
      // Skipping roll, but roll would happen here
      auto pitched_vec =
          Vec3<double>{dir_vector[0],
                       dir_vector[1] * std::cos(camera_pitch) -
                           dir_vector[2] * std::sin(camera_pitch),
                       dir_vector[1] * std::sin(camera_pitch) +
                           dir_vector[2] * std::cos(camera_pitch)};

      auto rotated_dir_vec = Vec3<double>{
          pitched_vec[0] * std::cos(camera_yaw) +
              pitched_vec[2] * std::sin(camera_yaw),
          pitched_vec[1],
          pitched_vec[2] * std::cos(camera_yaw) -
              pitched_vec[0] * std::sin(camera_yaw),
      };

      // normalise rotated_dir_vec
      auto dir_vec_mod = std::sqrt(rotated_dir_vec[0] * rotated_dir_vec[0] +
                                   rotated_dir_vec[1] * rotated_dir_vec[1] +
                                   rotated_dir_vec[2] * rotated_dir_vec[2]);

      rotated_dir_vec = {rotated_dir_vec[0] / dir_vec_mod,
                         rotated_dir_vec[1] / dir_vec_mod,
                         rotated_dir_vec[2] / dir_vec_mod};

      auto has_hit_object = -1;
      auto lambda_mu = std::pair<std::size_t, size_t>{0, 0};

      // Establish the line and determine if it hits an object
      for (auto face_i = 0; face_i < faces.size(); face_i++) {

        auto face = Face<T>{faces[face_i]};
        auto face_n_and_d = pre_calculated_face_normals_and_ds[face_i];

        // Get the point of intersection
        // Abbreviations to make equation shorter:
        // (a + lambda b) dot n = d
        // solving for lambda
        auto &a = camera.origin;
        auto &b = rotated_dir_vec;
        auto &n = face_n_and_d.first;
        auto &d = face_n_and_d.second;

        // Direction vector must not be parallel with surface normal
        if (std::abs(b[0] * n[0] + b[1] * n[1] + b[2] * n[2]) < 1e-06)
          continue;

        auto lambda = (d - (a[0] * n[0] + a[1] * n[1] + a[2] * n[2])) /
                      (b[0] * n[0] + b[1] * n[1] + b[2] * n[2]);

        if (lambda < 0)
          continue;

        auto point_of_intersection = a + (lambda * b);

        // Determine if it actually lies in the Face
        // auto indexes = relevant_coords_per_plane[face_i];

        auto local_point_on_surface = point_of_intersection - face.origin;

        auto face_a_mod =
            std::sqrt(face.a[0] * face.a[0] + face.a[1] * face.a[1] +
                      face.a[2] * face.a[2]);
        auto face_b_mod =
            std::sqrt(face.b[0] * face.b[0] + face.b[1] * face.b[1] +
                      face.b[2] * face.b[2]);

        // As well as perpendicular surfaces, dot product can also indicate in
        // this scenario How much as a scalar the local position is made up of
        // the dotted vector
        //
        // EG: LocalPos dot A -> how much of LocalPos is made up by A
        auto lambda_dot =
            T{local_point_on_surface[0] * (face.a[0] / face_a_mod) +
              local_point_on_surface[1] * (face.a[1] / face_a_mod) +
              local_point_on_surface[2] * (face.a[2] / face_a_mod)};

        auto mu_dot = T{local_point_on_surface[0] * (face.b[0] / face_b_mod) +
                        local_point_on_surface[1] * (face.b[1] / face_b_mod) +
                        local_point_on_surface[2] * (face.b[2] / face_b_mod)};

        if (!(0 < lambda_dot && lambda_dot <= face_a_mod))
          continue;

        if (!(0 < mu_dot && mu_dot <= face_b_mod))
          continue;

        // Has hit an object
        has_hit_object = face_i;
        lambda_mu = {lambda_dot, mu_dot};

        break;
      }

      if (has_hit_object != -1) {
        pixel_buffer.push_back(
            image_buffers[has_hit_object]
                         [(static_cast<std::size_t>(lambda_mu.first) *
                           image_buffer_dims[has_hit_object].first * 3) +
                          (3 * lambda_mu.second) + 0]);
        pixel_buffer.push_back(
            image_buffers[has_hit_object]
                         [(static_cast<std::size_t>(lambda_mu.first) *
                           image_buffer_dims[has_hit_object].first * 3) +
                          (3 * lambda_mu.second) + 1]);
        pixel_buffer.push_back(
            image_buffers[has_hit_object]
                         [(static_cast<std::size_t>(lambda_mu.first) *
                           image_buffer_dims[has_hit_object].first * 3) +
                          (3 * lambda_mu.second) + 2]);
      } else {
        pixel_buffer.emplace_back(120);
        pixel_buffer.emplace_back(120);
        pixel_buffer.emplace_back(120);
      }
    }
  }

  return pixel_buffer;
}

} // namespace Cuboid

#endif
