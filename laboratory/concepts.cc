#include <type_traits>
#include <concepts>
#include <print>

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <Arithmetic T>
T square(T x) {
  return x*x;
}

template <typename T>
concept Drawable = requires(T t) {
  { t.draw() } -> std::same_as<void>;
};

template <typename T>
concept Movable = requires(T t) {
  { t.move() } -> std::same_as<void>;
};

template <typename T>
concept GameEntity = Drawable<T> && Movable<T>;

struct Player {
  void draw() { std::println("Drawing player"); }
  void move() { std::println("Moving player"); }
};

template <GameEntity T>
void update(T& entity) {
  entity.move();
  entity.draw();
}

int main() {
  square(123);
  // square("1212"); - error
  square(1.23);

  Player pl;
  update(pl);
  return 0;
}
