#include "SnakeSegmentActor.h"

namespace snake {

using CollisionLayer = pr32::physics::CollisionLayer;

static constexpr CollisionLayer LAYER_SNAKE_HEAD = 1 << 0;
static constexpr CollisionLayer LAYER_SNAKE_BODY = 1 << 1;

SnakeSegmentActor::SnakeSegmentActor(int gridX, int gridY, bool head)
    : pr32::core::Actor(gridX * CELL_SIZE, gridY * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1),
      cellX(gridX),
      cellY(gridY),
      isHead(head),
      alive(true) {
    setHead(head);
}

void SnakeSegmentActor::update(unsigned long deltaTime) {
    (void)deltaTime;
}

void SnakeSegmentActor::draw(pr32::graphics::Renderer& renderer) {
    using Color = pr32::graphics::Color;
    Color color = isHead ? Color::LightGreen : Color::DarkGreen;
    renderer.drawFilledRectangle(cellX * CELL_SIZE, cellY * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, color);
}

pr32::core::Rect SnakeSegmentActor::getHitBox() {
    return { static_cast<float>(cellX * CELL_SIZE), static_cast<float>(cellY * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1 };
}

void SnakeSegmentActor::onCollision(pr32::core::Actor* other) {
    (void)other;
    if (isHead) {
        alive = false;
    }
}

void SnakeSegmentActor::setCellPosition(int gridX, int gridY) {
    cellX = gridX;
    cellY = gridY;
}

int SnakeSegmentActor::getCellX() const {
    return cellX;
}

int SnakeSegmentActor::getCellY() const {
    return cellY;
}

void SnakeSegmentActor::setHead(bool head) {
    isHead = head;
    if (isHead) {
        setCollisionLayer(LAYER_SNAKE_HEAD);
        setCollisionMask(LAYER_SNAKE_BODY);
    } else {
        setCollisionLayer(LAYER_SNAKE_BODY);
        setCollisionMask(0);
    }
}

void SnakeSegmentActor::resetAlive() {
    alive = true;
}

bool SnakeSegmentActor::isAlive() const {
    return alive;
}

}

