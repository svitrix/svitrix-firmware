#include "GameEffects.h"

// ---- Looking Eyes ----

namespace
{

constexpr int kBlinkFrames[] = {1, 2, 3, 4, 3, 2, 1};
constexpr int kBlinkFrameCount = sizeof(kBlinkFrames) / sizeof(kBlinkFrames[0]);

struct EyeState
{
    int blinkCountdown = 60;
    int gazeCountdown = 30;
    int gazeFrames = 5;
    int eyeX = 9, eyeY = 3;
    int newX = 9, newY = 3;
    int dX = 6, dY = 0;
    int mood = 0;
};

EyeState eyeState;

const uint16_t eyeFrames[5][64] = {
    {0, 0, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

} // namespace

void LookingEyes(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *)
{
    auto& e = eyeState;

    if (e.blinkCountdown < kBlinkFrameCount - 1)
    {
        canvas.drawRGBBitmap(6 + x, y, eyeFrames[kBlinkFrames[e.blinkCountdown]], 8, 8);
        canvas.drawRGBBitmap(18 + x, y, eyeFrames[kBlinkFrames[e.blinkCountdown]], 8, 8);
    }
    else
    {
        canvas.drawRGBBitmap(6 + x, y, eyeFrames[0], 8, 8);
        canvas.drawRGBBitmap(18 + x, y, eyeFrames[0], 8, 8);
    }

    --e.blinkCountdown;
    if (e.blinkCountdown == 0)
        e.blinkCountdown = random(60, 350);

    if (e.gazeCountdown <= e.gazeFrames)
    {
        --e.gazeCountdown;
        int gx = e.newX - (e.dX * e.gazeCountdown / e.gazeFrames);
        int gy = e.newY - (e.dY * e.gazeCountdown / e.gazeFrames);
        canvas.fillRect(gx + 6 + x, gy + y, 2, 2, 0);
        canvas.fillRect(gx + 18 + x, gy + y, 2, 2, 0);

        if (e.gazeCountdown == 0)
        {
            e.eyeX = e.newX;
            e.eyeY = e.newY;
            do
            {
                switch (e.mood)
                {
                case 0:
                    e.newX = random(0, 6);
                    e.newY = random(0, 6);
                    e.dX = e.newX - 4;
                    e.dY = e.newY - 4;
                    break;
                case 1:
                    e.newX = random(0, 7);
                    e.newY = random(0, 7);
                    e.dX = e.newX - 3;
                    e.dY = e.newY - 3;
                    break;
                case 2:
                    e.newX = random(1, 7);
                    e.newY = random(1, 4);
                    e.dX = e.newX - 3;
                    e.dY = e.newY - 3;
                    break;
                case 3:
                    e.newX = random(0, 7);
                    e.newY = random(3, 7);
                    e.dX = e.newX - 3;
                    e.dY = e.newY - 3;
                    break;
                }
            } while (e.dX * e.dX + e.dY * e.dY <= 3);

            e.dX = e.newX - e.eyeX;
            e.dY = e.newY - e.eyeY;
            e.gazeFrames = random(15, 40);
            e.gazeCountdown = random(e.gazeFrames, 120);
        }
    }
    else
    {
        --e.gazeCountdown;
        canvas.fillRect(e.eyeX + 6 + x, e.eyeY + y, 2, 2, 0);
        canvas.fillRect(e.eyeX + 18 + x, e.eyeY + y, 2, 2, 0);
    }
}

// ---- Snake ----

namespace
{

constexpr uint8_t kMaxSnakeLength = 64;
constexpr uint8_t kInitialSnakeLength = 2;

struct SnakePoint
{
    int16_t x, y;
    uint8_t colorIndex;
};

struct SnakeState
{
    SnakePoint body[kMaxSnakeLength];
    bool shouldMove[kMaxSnakeLength];
    uint8_t length = kInitialSnakeLength;
    uint8_t direction = 0; // 0=up, 1=right, 2=down, 3=left
    uint32_t lastUpdate = 0;
    bool gameOver = false;
    SnakePoint apple;
    uint8_t colorIndex = 0;
};

SnakeState snakeState;

bool willCollide(const SnakeState& s, int16_t px, int16_t py)
{
    for (uint8_t i = 1; i < s.length; i++)
        if (px == s.body[i].x && py == s.body[i].y)
            return true;
    return false;
}

void updateSnakeDirection(SnakeState& s)
{
    auto& head = s.body[0];

    if (s.direction % 2 == 0)
    {
        if (s.apple.y == head.y)
        {
            if (s.apple.x > head.x && s.direction != 3)
                s.direction = 1;
            else if (s.apple.x < head.x && s.direction != 1)
                s.direction = 3;
        }
    }
    else
    {
        if (s.apple.x == head.x)
        {
            if (s.apple.y > head.y && s.direction != 0)
                s.direction = 2;
            else if (s.apple.y < head.y && s.direction != 2)
                s.direction = 0;
        }
    }

    // Predict next position and avoid self-collision
    SnakePoint next = head;
    switch (s.direction)
    {
    case 0:
        next.y--;
        break;
    case 1:
        next.x++;
        break;
    case 2:
        next.y++;
        break;
    case 3:
        next.x--;
        break;
    }

    for (uint8_t i = 1; i < s.length; i++)
    {
        if (next.x == s.body[i].x && next.y == s.body[i].y)
        {
            if (s.direction % 2 == 0)
            {
                if (s.direction != 1 && head.x + 1 < kMatrixWidth && !willCollide(s, head.x + 1, head.y))
                    s.direction = 1;
                else if (s.direction != 3 && head.x - 1 >= 0 && !willCollide(s, head.x - 1, head.y))
                    s.direction = 3;
            }
            else
            {
                if (s.direction != 0 && head.y + 1 < kMatrixHeight && !willCollide(s, head.x, head.y + 1))
                    s.direction = 2;
                else if (s.direction != 2 && head.y - 1 >= 0 && !willCollide(s, head.x, head.y - 1))
                    s.direction = 0;
            }
        }
    }
}

} // namespace

void SnakeGame(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    auto& s = snakeState;

    if (s.gameOver)
    {
        for (uint8_t i = 0; i < kInitialSnakeLength; i++)
        {
            s.body[i] = {0, static_cast<int16_t>(i), s.colorIndex};
            s.shouldMove[i] = true;
        }
        s.direction = 1;
        s.gameOver = false;
        s.length = kInitialSnakeLength;
        s.apple = {static_cast<int16_t>(random(kMatrixWidth)), static_cast<int16_t>(random(kMatrixHeight)), 0};
    }

    if (millis() - s.lastUpdate > 150 - settings->speed * 10)
    {
        s.lastUpdate = millis();
        updateSnakeDirection(s);

        // Eat apple
        if (s.body[0].x == s.apple.x && s.body[0].y == s.apple.y)
        {
            if (s.length < kMaxSnakeLength)
            {
                s.body[s.length] = s.apple;
                s.shouldMove[s.length] = false;
                s.length++;
            }
            s.apple = {static_cast<int16_t>(random(kMatrixWidth)), static_cast<int16_t>(random(kMatrixHeight)), 0};
        }

        // Move body
        for (uint8_t i = s.length - 1; i > 0; i--)
        {
            if (s.shouldMove[i])
                s.body[i] = s.body[i - 1];
            else
                s.shouldMove[i] = true;
        }

        // Move head
        switch (s.direction)
        {
        case 0:
            s.body[0].y--;
            break;
        case 1:
            s.body[0].x++;
            break;
        case 2:
            s.body[0].y++;
            break;
        case 3:
            s.body[0].x--;
            break;
        }

        // Wrap around
        auto& head = s.body[0];
        if (head.x < 0)
            head.x = kMatrixWidth - 1;
        else if (head.x >= kMatrixWidth)
            head.x = 0;
        else if (head.y < 0)
            head.y = kMatrixHeight - 1;
        else if (head.y >= kMatrixHeight)
            head.y = 0;

        // Update color on wrap
        if (head.x == 0 || head.x == kMatrixWidth - 1 || head.y == 0 || head.y == kMatrixHeight - 1)
        {
            s.colorIndex = (s.colorIndex + 10) % 255;
            head.colorIndex = s.colorIndex;
        }

        // Self-collision check
        for (uint8_t i = 1; i < s.length; i++)
            if (head.x == s.body[i].x && head.y == s.body[i].y)
                s.gameOver = true;
    }

    // Draw
    for (uint8_t i = 0; i < s.length; i++)
    {
        CRGB color = ColorFromPalette(settings->palette, s.body[i].colorIndex, 255, blendMode(settings));
        canvas.drawPixel(x + s.body[i].x, y + s.body[i].y, color);
    }
    canvas.drawPixel(x + s.apple.x, y + s.apple.y, canvas.Color(255, 0, 0));
}

// ---- Ping Pong ----

namespace
{

constexpr int kPaddleHeight = 3;
constexpr int kBallSize = 1;

struct PongState
{
    struct
    {
        int y, dy;
    } paddle1{kMatrixHeight / 2, 1}, paddle2{kMatrixHeight / 2, -1};
    struct
    {
        int x, y, dx, dy;
    } ball{kMatrixWidth / 2, kMatrixHeight / 2, 1, 1};
    uint32_t lastUpdate = 0;
};

PongState pongState;

} // namespace

void PingPongEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    auto& s = pongState;

    if (millis() - s.lastUpdate > 150 - settings->speed * 10)
    {
        s.lastUpdate = millis();

        s.paddle1.y += s.paddle1.dy;
        s.paddle2.y += s.paddle2.dy;

        if (s.paddle1.y <= 0 || s.paddle1.y + kPaddleHeight >= kMatrixHeight)
            s.paddle1.dy = -s.paddle1.dy;
        if (s.paddle2.y <= 0 || s.paddle2.y + kPaddleHeight >= kMatrixHeight)
            s.paddle2.dy = -s.paddle2.dy;

        s.ball.x += s.ball.dx;
        s.ball.y += s.ball.dy;

        if (s.ball.y <= 0 || s.ball.y + kBallSize >= kMatrixHeight)
            s.ball.dy = -s.ball.dy;

        // Reset if ball passes a paddle
        if ((s.ball.x < 0 && (s.ball.y < s.paddle1.y || s.ball.y > s.paddle1.y + kPaddleHeight)) ||
            (s.ball.x + kBallSize > kMatrixWidth && (s.ball.y < s.paddle2.y || s.ball.y > s.paddle2.y + kPaddleHeight)))
        {
            s.ball = {kMatrixWidth / 2, kMatrixHeight / 2, 1, 1};
        }

        // Bounce off paddles
        if (s.ball.x <= 0 && s.ball.y >= s.paddle1.y && s.ball.y < s.paddle1.y + kPaddleHeight)
            s.ball.dx = -s.ball.dx;
        if (s.ball.x + kBallSize >= kMatrixWidth && s.ball.y >= s.paddle2.y && s.ball.y < s.paddle2.y + kPaddleHeight)
            s.ball.dx = -s.ball.dx;
    }

    // Draw paddles
    for (int i = 0; i < kPaddleHeight; i++)
    {
        canvas.drawPixel(x, y + s.paddle1.y + i, canvas.Color(255, 255, 255));
        canvas.drawPixel(x + kMatrixWidth - 1, y + s.paddle2.y + i, canvas.Color(255, 255, 255));
    }
    // Draw ball
    canvas.drawPixel(x + s.ball.x, y + s.ball.y, canvas.Color(255, 0, 0));
}

// ---- Brick Breaker ----

namespace
{

constexpr int kBrickPaddleWidth = 3;
constexpr int kBrickCols = 16;
constexpr int kBrickRows = 3;

struct BrickState
{
    struct
    {
        int x, dx;
    } paddle{kMatrixWidth / 2, 1};
    struct
    {
        int x, y, dx, dy;
    } ball{kMatrixWidth / 2, kMatrixHeight / 2, 1, 1};
    bool bricks[kBrickRows][kBrickCols];
    bool initialized = false;
    uint32_t lastUpdate = 0;

    void resetBricks()
    {
        for (auto& row : bricks)
            for (auto& b : row)
                b = true;
    }

    bool anyBricksRemain() const
    {
        for (const auto& row : bricks)
            for (bool b : row)
                if (b)
                    return true;
        return false;
    }
};

BrickState brickState;

} // namespace

void BrickBreakerEffect(IPixelCanvas& canvas, int16_t x, int16_t y, EffectSettings *settings)
{
    auto& s = brickState;

    if (!s.initialized)
    {
        s.resetBricks();
        s.initialized = true;
    }

    if (millis() - s.lastUpdate > 150 - settings->speed * 10)
    {
        s.lastUpdate = millis();

        s.ball.x += s.ball.dx;
        s.ball.y += s.ball.dy;
        s.paddle.x = s.ball.x - 1;

        if (s.paddle.x <= 0 || s.paddle.x + kBrickPaddleWidth >= kMatrixWidth)
            s.paddle.dx = -s.paddle.dx;

        if (s.ball.x <= 0 || s.ball.x + 1 >= kMatrixWidth)
            s.ball.dx = -s.ball.dx;
        if (s.ball.y <= 0 || s.ball.y + 1 >= kMatrixHeight)
            s.ball.dy = -s.ball.dy;

        // Brick collision
        for (int i = 0; i < kBrickRows; i++)
        {
            for (int j = 0; j < kBrickCols; j++)
            {
                if (s.bricks[i][j] && s.ball.y == i &&
                    s.ball.x >= j * (kMatrixWidth / kBrickCols) &&
                    s.ball.x < (j + 1) * (kMatrixWidth / kBrickCols))
                {
                    s.bricks[i][j] = false;
                    s.ball.dy = -s.ball.dy;
                }
            }
        }

        if (!s.anyBricksRemain())
            s.resetBricks();
    }

    // Draw paddle
    for (int i = 0; i < kBrickPaddleWidth; i++)
        canvas.drawPixel(x + s.paddle.x + i, y + kMatrixHeight - 1, canvas.Color(255, 255, 255));

    // Draw ball
    canvas.drawPixel(x + s.ball.x, y + s.ball.y, canvas.Color(255, 0, 0));

    // Draw bricks
    for (int i = 0; i < kBrickRows; i++)
        for (int j = 0; j < kBrickCols; j++)
            if (s.bricks[i][j])
                canvas.drawPixel(x + j * (kMatrixWidth / kBrickCols), y + i, canvas.Color(0, 0, 255));
}

// ---- Reset all game effect state ----

void resetGameEffectState()
{
    eyeState = EyeState{};
    snakeState = SnakeState{};
    snakeState.gameOver = true; // Force re-init on next call
    pongState = PongState{};
    brickState = BrickState{};
}
