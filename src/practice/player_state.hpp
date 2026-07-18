#pragma once

#include <Geode/Geode.hpp>
#include <array>
#include <unordered_map>
#include <vector>

// Snapshot of PlayerObject fields needed for stable practice restores.
struct PlayerSnapshot {
    static PlayerSnapshot capture(PlayerObject* p);
    void restore(PlayerObject* p) const;

    cocos2d::CCPoint position{};
    float rotation = 0.f;

    bool wasTeleported = false;
    bool fixGravityBug = false;
    bool reverseSync = false;

    double yVelocityBeforeSlope = 0;
    double dashX = 0, dashY = 0, dashAngle = 0, dashStartTime = 0;
    DashRingObject* dashRing = nullptr;
    double slopeStartTime = 0;
    bool justPlacedStreak = false;
    GameObject* maybeLastGroundObject = nullptr;

    int lastCollisionBottom = 0, lastCollisionTop = 0;
    int lastCollisionLeft = 0, lastCollisionRight = 0;
    int unk50C = 0, unk510 = 0;

    GameObject* currentSlope2 = nullptr;
    GameObject* preLastGroundObject = nullptr;
    float slopeAngle = 0;
    bool slopeSlidingMaybeRotated = false;
    bool quickCheckpointMode = false;
    GameObject* collidedObject = nullptr;
    GameObject* lastGroundObject = nullptr;
    GameObject* collidingWithLeft = nullptr;
    GameObject* collidingWithRight = nullptr;
    int maybeSavedPlayerFrame = 0;

    double scaleXRelated2 = 0, groundYVelocity = 0, yVelocityRelated = 0;
    double scaleXRelated3 = 0, scaleXRelated4 = 0, scaleXRelated5 = 0;
    bool isCollidingWithSlope = false;
    bool isBallRotating = false;
    bool unk669 = false;
    GameObject* currentPotentialSlope = nullptr;
    GameObject* currentSlope = nullptr;
    double unk584 = 0;
    int collidingWithSlopeId = 0;
    bool slopeFlipGravityRelated = false;

    float slopeAngleRadians = 0;
    float rotationSpeed = 0, rotateSpeed = 0;
    bool isRotating = false, isBallRotating2 = false;
    bool hasGlow = false, isHidden = false;

    double speedMultiplier = 0, yStart = 0, gravity = 0;
    float trailingParticleLife = 0, unk648 = 0;
    double gameModeChangedTime = 0;
    bool padRingRelated = false, maybeReducedEffects = false;
    bool maybeIsFalling = false, shouldTryPlacingCheckpoint = false;
    bool playEffects = false, maybeCanRunIntoBlocks = false;
    bool hasGroundParticles = false, hasShipParticles = false;
    bool isOnGround3 = false, checkpointTimeout = false;

    double lastCheckpointTime = 0, lastJumpTime = 0, lastFlipTime = 0;
    double flashTime = 0;
    float flashDuration = 0, flashDelay = 0;
    double lastSpiderFlipTime = 0;
    bool unkBool5 = false, maybeIsVehicleGlowing = false;
    bool switchWaveTrailColor = false, practiceDeathEffect = false;

    double accelerationOrSpeed = 0, snapDistance = 0;
    bool ringJumpRelated = false;
    GameObject* objectSnappedTo = nullptr;
    CheckpointObject* pendingCheckpoint = nullptr;
    int onFlyCheckpointTries = 0;

    bool jumpBuffered = false, stateRingJump = false;
    bool wasJumpBuffered = false, wasRobotJump = false;
    unsigned char stateJumpBuffered = 0;
    bool stateRingJump2 = false, touchedRing = false, touchedCustomRing = false;
    bool touchedGravityPortal = false, maybeTouchedBreakableBlock = false;
    bool touchedPad = false;

    double yVelocity = 0, fallSpeed = 0;
    bool isOnSlope = false, wasOnSlope = false;
    float slopeVelocity = 0;
    bool maybeUpsideDownSlope = false;

    bool isShip = false, isBird = false, isBall = false, isDart = false;
    bool isRobot = false, isSpider = false, isUpsideDown = false;
    bool isDead = false, isOnGround = false, isGoingLeft = false;
    bool isSideways = false, isSwing = false;

    int reverseRelated = 0;
    double maybeReverseSpeed = 0, maybeReverseAcceleration = 0;
    float xVelocityRelated2 = 0;
    bool isDashing = false;
    int groundObjectMaterial = 0;
    float vehicleSize = 0, playerSpeed = 0;
    cocos2d::CCPoint shipRotation{};
    cocos2d::CCPoint lastPortalPos{};
    bool isOnGround2 = false;
    double lastLandTime = 0;
    float platformerVelocityRelated = 0;
    bool maybeIsBoosted = false;
    double scaleXRelatedTime = 0;
    bool decreaseBoostSlide = false, isLocked = false, controlsDisabled = false;
    cocos2d::CCPoint lastGroundedPos{};

    GameObject* lastActivatedPortal = nullptr;
    bool hasEverJumped = false;
    double totalTime = 0;
    bool isSecondPlayer = false;
    bool isBeingSpawnedByDualPortal = false;

    double platformerXVelocity = 0;
    bool holdingRight = false, holdingLeft = false, leftPressedFirst = false;
    double scaleXRelated = 0;
    bool maybeHasStopped = false;
    float xVelocityRelated = 0;
    bool maybeGoingCorrectSlopeDirection = false, isSliding = false;
    double maybeSlopeForce = 0;
    bool isOnIce = false;
    double physDeltaRelated = 0;
    bool isOnGround4 = false;
    int maybeSlidingTime = 0;
    double maybeSlidingStartTime = 0, changedDirectionsTime = 0, slopeEndTime = 0;
    bool isMoving = false, platformerMovingLeft = false, platformerMovingRight = false;
    bool isSlidingRight = false;
    double maybeChangedDirectionAngle = 0;
    bool isPlatformer = false;

    float gravityMod = 0;
    bool affectedByForces = false;
    bool fixRobotJump = false;
    gd::map<int, bool> holdingButtons;
    bool inputsLocked = false;
    bool isOutOfBounds = false;
    float fallStartY = 0;
    bool ignoreDamage = false;
    bool enable22Changes = false;
};

struct PracticeCheckpoint {
    PlayerSnapshot p1;
    PlayerSnapshot p2;
    CheckpointObject* object = nullptr;

    size_t platformerStack = 0;
    uint64_t attemptStartFrame = 0;
    uint64_t frame = 0;
    uint64_t seedState = 0;
    uint64_t shakeState = 0;
    double tps = 240.0;
    double warpedTime = 0.0;

    std::unordered_map<int, int> persistentItems;
    std::array<float, 2000> variance{};
    std::vector<GameObject*> calcNonEffectObjects;
    int calcNonEffectObjectsSize = 0;
    std::vector<GameObject*> brokenObjects;
};
