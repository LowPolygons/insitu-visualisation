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

/*
 * WARN: An assumption has to be made here about the order of the faces and how
 * theyve been inserted
 * - The first face will be parallel with the XY plane (vert lying)
 * - The second face will be parallel with the YZ plane (vert lying)
 * - The third will be parallel with the XZ plane (horiz lying)
 */
template <typename T>
auto get_faces_of_cuboid(
    const std::array<std::pair<std::size_t, std::size_t>, 3> &faces)
    -> std::array<Face<T>, 3> {
  // The cuboid has its front left bottom corner on 0,0,0 and extends right in
  // the X, up the in the Y and forward in the Z
  //
  // The direction vectors for the face should only have one non-zero
  // coefficient

  return {Face<T>{{T{0}, T{0}, T{0}},
                  {faces[0].first, T{0}, T{0}},
                  {T{0}, faces[0].second, T{0}}},
          Face<T>{{faces[0].first, T{0}, T{0}},
                  {T{0}, T{0}, faces[1].first},
                  {T{0}, faces[1].second, T{0}}},
          Face<T>{// WARN: May need to swap the first two lines
                  {T{0}, faces[0].second, T{0}},
                  {faces[2].second, T{0}, T{0}},
                  {T{0}, T{0}, faces[2].first}}};
}

template <typename T>
auto get_position_camera(const std::array<Face<T>, 3> &faces) -> Camera<T> {
  // WARN: Make sure these .first and .second are correct
  std::cout << "Hoz options: " << faces[0].a[0] << ", " << faces[1].a[2]
            << std::endl;
  auto max_hoz_distance = std::max(faces[0].a[0], faces[1].a[2]);
  auto shorter_hoz_distance = std::min(faces[0].a[0], faces[1].a[2]);

  // WARN: Temporary -45 degree normalsied
  // auto camera_angle = Vec3<T>{T{-1}, -0.25, T{1}};
  auto camera_angle = Vec3<T>{-0.75, 0.5, 1};

  auto mod = std::sqrt(camera_angle[0] * camera_angle[0] +
                       camera_angle[1] * camera_angle[1] +
                       camera_angle[2] * camera_angle[2]);

  camera_angle = {camera_angle[0] / mod, camera_angle[1] / mod,
                  camera_angle[2] / mod};

  return Camera<T>{
      {max_hoz_distance * 2, 1.25 * faces[0].b[1], -2 * shorter_hoz_distance},
      camera_angle};
}

template <typename T>
auto get_pixel_buffer(std::pair<std::size_t, std::size_t> image_dimensions,
                      Camera<T> camera, std::array<Face<T>, 3> faces)
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

  // Camera Pitch and Yaw with (0,0,1) being a forward vector
  // WARN: making the assumption the direction vector is normalised
  auto camera_yaw = std::atan2(camera.direction[0], camera.direction[2]);
  auto camera_pitch =
      std::atan2(camera.direction[1],
                 std::sqrt(camera.direction[0] * camera.direction[0] +
                           camera.direction[2] * camera.direction[2]));

  std::cout << "Camera Yaw: " << camera_yaw << std::endl;
  std::cout << "Camera Pitch: " << camera_pitch << std::endl;

  /*
   *
   * A constexpr array indicating which are the 2 relevant coefficients for
   * implicit barycentrics
   *
   */
  constexpr auto relevant_coords_per_plane =
      std::array<std::pair<std::size_t, std::size_t>, 3>{{
          {0, 1},
          {2, 1},
          {2, 0},
      }};

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

      // Establish the line and determine if it hits an object
      for (auto face_i = 0; face_i < faces.size(); face_i++) {

        auto face = faces[face_i];
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
        auto indexes = relevant_coords_per_plane[face_i];

        if (pixel_x == 500 && pixel_y == 500) {
          std::cout << "///////////////////" << std::endl;
          std::cout << "INDEX: " << face_i
                    << ", P.o.I: " << stringVec(point_of_intersection)
                    << std::endl;
          std::cout << "A: " << stringVec(a) << std::endl;
          std::cout << "Lambda: " << lambda << std::endl;
          std::cout << "B: " << stringVec(b) << std::endl;

          std::cout << point_of_intersection[indexes.first] << " against "
                    << std::abs(face.a[indexes.first]) << std::endl;
          std::cout << point_of_intersection[indexes.second] << " against "
                    << std::abs(face.b[indexes.second]) << std::endl;

          std::cout << "///////////////////" << std::endl;
        }

        auto local_point_on_surface = point_of_intersection;

        if (local_point_on_surface[indexes.first] <= 0 ||
            local_point_on_surface[indexes.first] >
                std::abs(face.a[indexes.first]))
          continue;

        if (local_point_on_surface[indexes.second] <= 0 ||
            local_point_on_surface[indexes.second] >
                std::abs(face.b[indexes.second]))
          continue;

        // Has hit an object
        has_hit_object = face_i;
        break;
      }

      if (has_hit_object != -1) {
        switch (has_hit_object) {
        case 0: {
          pixel_buffer.emplace_back(10);
          pixel_buffer.emplace_back(10);
          pixel_buffer.emplace_back(10);
          break;
        }
        case 1: {
          pixel_buffer.emplace_back(100);
          pixel_buffer.emplace_back(200);
          pixel_buffer.emplace_back(100);
          break;
        }
        case 2: {
          pixel_buffer.emplace_back(200);
          pixel_buffer.emplace_back(200);
          pixel_buffer.emplace_back(200);
          break;
        }
        }
      } else {
        pixel_buffer.emplace_back(255);
        pixel_buffer.emplace_back(255);
        pixel_buffer.emplace_back(255);
      }
    }
  }

  return pixel_buffer;
}

} // namespace Cuboid

#endif
