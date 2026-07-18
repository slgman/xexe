#include "practice/player_state.hpp"

PlayerSnapshot PlayerSnapshot::capture(PlayerObject* p) {
    PlayerSnapshot s;
    s.position = p->m_position;
    s.rotation = p->getRotation();

    s.wasTeleported = p->m_wasTeleported;
    s.fixGravityBug = p->m_fixGravityBug;
    s.reverseSync = p->m_reverseSync;
    s.yVelocityBeforeSlope = p->m_yVelocityBeforeSlope;
    s.dashX = p->m_dashX;
    s.dashY = p->m_dashY;
    s.dashAngle = p->m_dashAngle;
    s.dashStartTime = p->m_dashStartTime;
    s.dashRing = p->m_dashRing;
    s.slopeStartTime = p->m_slopeStartTime;
    s.justPlacedStreak = p->m_justPlacedStreak;
    s.maybeLastGroundObject = p->m_maybeLastGroundObject;

    s.lastCollisionBottom = p->m_lastCollisionBottom;
    s.lastCollisionTop = p->m_lastCollisionTop;
    s.lastCollisionLeft = p->m_lastCollisionLeft;
    s.lastCollisionRight = p->m_lastCollisionRight;
    s.unk50C = p->m_unk50C;
    s.unk510 = p->m_unk510;

    s.currentSlope2 = p->m_currentSlope2;
    s.preLastGroundObject = p->m_preLastGroundObject;
    s.slopeAngle = p->m_slopeAngle;
    s.slopeSlidingMaybeRotated = p->m_slopeSlidingMaybeRotated;
    s.quickCheckpointMode = p->m_quickCheckpointMode;
    s.collidedObject = p->m_collidedObject;
    s.lastGroundObject = p->m_lastGroundObject;
    s.collidingWithLeft = p->m_collidingWithLeft;
    s.collidingWithRight = p->m_collidingWithRight;
    s.maybeSavedPlayerFrame = p->m_maybeSavedPlayerFrame;

    s.scaleXRelated2 = p->m_scaleXRelated2;
    s.groundYVelocity = p->m_groundYVelocity;
    s.yVelocityRelated = p->m_yVelocityRelated;
    s.scaleXRelated3 = p->m_scaleXRelated3;
    s.scaleXRelated4 = p->m_scaleXRelated4;
    s.scaleXRelated5 = p->m_scaleXRelated5;
    s.isCollidingWithSlope = p->m_isCollidingWithSlope;
    s.isBallRotating = p->m_isBallRotating;
    s.unk669 = p->m_unk669;
    s.currentPotentialSlope = p->m_currentPotentialSlope;
    s.currentSlope = p->m_currentSlope;
    s.unk584 = p->unk_584;
    s.collidingWithSlopeId = p->m_collidingWithSlopeId;
    s.slopeFlipGravityRelated = p->m_slopeFlipGravityRelated;

    s.slopeAngleRadians = p->m_slopeAngleRadians;
    s.rotationSpeed = p->m_rotationSpeed;
    s.rotateSpeed = p->m_rotateSpeed;
    s.isRotating = p->m_isRotating;
    s.isBallRotating2 = p->m_isBallRotating2;
    s.hasGlow = p->m_hasGlow;
    s.isHidden = p->m_isHidden;

    s.speedMultiplier = p->m_speedMultiplier;
    s.yStart = p->m_yStart;
    s.gravity = p->m_gravity;
    s.trailingParticleLife = p->m_trailingParticleLife;
    s.unk648 = p->m_unk648;
    s.gameModeChangedTime = p->m_gameModeChangedTime;
    s.padRingRelated = p->m_padRingRelated;
    s.maybeReducedEffects = p->m_maybeReducedEffects;
    s.maybeIsFalling = p->m_maybeIsFalling;
    s.shouldTryPlacingCheckpoint = p->m_shouldTryPlacingCheckpoint;
    s.playEffects = p->m_playEffects;
    s.maybeCanRunIntoBlocks = p->m_maybeCanRunIntoBlocks;
    s.hasGroundParticles = p->m_hasGroundParticles;
    s.hasShipParticles = p->m_hasShipParticles;
    s.isOnGround3 = p->m_isOnGround3;
    s.checkpointTimeout = p->m_checkpointTimeout;

    s.lastCheckpointTime = p->m_lastCheckpointTime;
    s.lastJumpTime = p->m_lastJumpTime;
    s.lastFlipTime = p->m_lastFlipTime;
    s.flashTime = p->m_flashTime;
    s.flashDuration = p->m_flashDuration;
    s.flashDelay = p->m_flashDelay;
    s.lastSpiderFlipTime = p->m_lastSpiderFlipTime;
    s.unkBool5 = p->m_unkBool5;
    s.maybeIsVehicleGlowing = p->m_maybeIsVehicleGlowing;
    s.switchWaveTrailColor = p->m_switchWaveTrailColor;
    s.practiceDeathEffect = p->m_practiceDeathEffect;

    s.accelerationOrSpeed = p->m_accelerationOrSpeed;
    s.snapDistance = p->m_snapDistance;
    s.ringJumpRelated = p->m_ringJumpRelated;
    s.objectSnappedTo = p->m_objectSnappedTo;
    s.pendingCheckpoint = p->m_pendingCheckpoint;
    s.onFlyCheckpointTries = p->m_onFlyCheckpointTries;

    s.jumpBuffered = p->m_jumpBuffered;
    s.stateRingJump = p->m_stateRingJump;
    s.wasJumpBuffered = p->m_wasJumpBuffered;
    s.wasRobotJump = p->m_wasRobotJump;
    s.stateJumpBuffered = p->m_stateJumpBuffered;
    s.stateRingJump2 = p->m_stateRingJump2;
    s.touchedRing = p->m_touchedRing;
    s.touchedCustomRing = p->m_touchedCustomRing;
    s.touchedGravityPortal = p->m_touchedGravityPortal;
    s.maybeTouchedBreakableBlock = p->m_maybeTouchedBreakableBlock;
    s.touchedPad = p->m_touchedPad;

    s.yVelocity = p->m_yVelocity;
    s.fallSpeed = p->m_fallSpeed;
    s.isOnSlope = p->m_isOnSlope;
    s.wasOnSlope = p->m_wasOnSlope;
    s.slopeVelocity = p->m_slopeVelocity;
    s.maybeUpsideDownSlope = p->m_maybeUpsideDownSlope;

    s.isShip = p->m_isShip;
    s.isBird = p->m_isBird;
    s.isBall = p->m_isBall;
    s.isDart = p->m_isDart;
    s.isRobot = p->m_isRobot;
    s.isSpider = p->m_isSpider;
    s.isUpsideDown = p->m_isUpsideDown;
    s.isDead = p->m_isDead;
    s.isOnGround = p->m_isOnGround;
    s.isGoingLeft = p->m_isGoingLeft;
    s.isSideways = p->m_isSideways;
    s.isSwing = p->m_isSwing;

    s.reverseRelated = p->m_reverseRelated;
    s.maybeReverseSpeed = p->m_maybeReverseSpeed;
    s.maybeReverseAcceleration = p->m_maybeReverseAcceleration;
    s.xVelocityRelated2 = p->m_xVelocityRelated2;
    s.isDashing = p->m_isDashing;
    s.groundObjectMaterial = p->m_groundObjectMaterial;
    s.vehicleSize = p->m_vehicleSize;
    s.playerSpeed = p->m_playerSpeed;
    s.shipRotation = p->m_shipRotation;
    s.lastPortalPos = p->m_lastPortalPos;
    s.isOnGround2 = p->m_isOnGround2;
    s.lastLandTime = p->m_lastLandTime;
    s.platformerVelocityRelated = p->m_platformerVelocityRelated;
    s.maybeIsBoosted = p->m_maybeIsBoosted;
    s.scaleXRelatedTime = p->m_scaleXRelatedTime;
    s.decreaseBoostSlide = p->m_decreaseBoostSlide;
    s.isLocked = p->m_isLocked;
    s.controlsDisabled = p->m_controlsDisabled;
    s.lastGroundedPos = p->m_lastGroundedPos;

    s.lastActivatedPortal = p->m_lastActivatedPortal;
    s.hasEverJumped = p->m_hasEverJumped;
    s.totalTime = p->m_totalTime;
    s.isSecondPlayer = p->m_isSecondPlayer;
    s.isBeingSpawnedByDualPortal = p->m_isBeingSpawnedByDualPortal;

    s.platformerXVelocity = p->m_platformerXVelocity;
    s.holdingRight = p->m_holdingRight;
    s.holdingLeft = p->m_holdingLeft;
    s.leftPressedFirst = p->m_leftPressedFirst;
    s.scaleXRelated = p->m_scaleXRelated;
    s.maybeHasStopped = p->m_maybeHasStopped;
    s.xVelocityRelated = p->m_xVelocityRelated;
    s.maybeGoingCorrectSlopeDirection = p->m_maybeGoingCorrectSlopeDirection;
    s.isSliding = p->m_isSliding;
    s.maybeSlopeForce = p->m_maybeSlopeForce;
    s.isOnIce = p->m_isOnIce;
    s.physDeltaRelated = p->m_physDeltaRelated;
    s.isOnGround4 = p->m_isOnGround4;
    s.maybeSlidingTime = p->m_maybeSlidingTime;
    s.maybeSlidingStartTime = p->m_maybeSlidingStartTime;
    s.changedDirectionsTime = p->m_changedDirectionsTime;
    s.slopeEndTime = p->m_slopeEndTime;
    s.isMoving = p->m_isMoving;
    s.platformerMovingLeft = p->m_platformerMovingLeft;
    s.platformerMovingRight = p->m_platformerMovingRight;
    s.isSlidingRight = p->m_isSlidingRight;
    s.maybeChangedDirectionAngle = p->m_maybeChangedDirectionAngle;
    s.isPlatformer = p->m_isPlatformer;

    s.gravityMod = p->m_gravityMod;
    s.affectedByForces = p->m_affectedByForces;
    s.fixRobotJump = p->m_fixRobotJump;
    s.holdingButtons = p->m_holdingButtons;
    s.inputsLocked = p->m_inputsLocked;
    s.isOutOfBounds = p->m_isOutOfBounds;
    s.fallStartY = p->m_fallStartY;
    s.ignoreDamage = p->m_ignoreDamage;
    s.enable22Changes = p->m_enable22Changes;
    return s;
}

void PlayerSnapshot::restore(PlayerObject* p) const {
    p->setPosition(position);
    p->setRotation(rotation);
    p->m_position = position;

    p->m_wasTeleported = wasTeleported;
    p->m_fixGravityBug = fixGravityBug;
    p->m_reverseSync = reverseSync;
    p->m_yVelocityBeforeSlope = yVelocityBeforeSlope;
    p->m_dashX = dashX;
    p->m_dashY = dashY;
    p->m_dashAngle = dashAngle;
    p->m_dashStartTime = dashStartTime;
    p->m_dashRing = dashRing;
    p->m_slopeStartTime = slopeStartTime;
    p->m_justPlacedStreak = justPlacedStreak;
    p->m_maybeLastGroundObject = maybeLastGroundObject;

    p->m_lastCollisionBottom = lastCollisionBottom;
    p->m_lastCollisionTop = lastCollisionTop;
    p->m_lastCollisionLeft = lastCollisionLeft;
    p->m_lastCollisionRight = lastCollisionRight;
    p->m_unk50C = unk50C;
    p->m_unk510 = unk510;

    p->m_currentSlope2 = currentSlope2;
    p->m_preLastGroundObject = preLastGroundObject;
    p->m_slopeAngle = slopeAngle;
    p->m_slopeSlidingMaybeRotated = slopeSlidingMaybeRotated;
    p->m_quickCheckpointMode = quickCheckpointMode;
    p->m_collidedObject = collidedObject;
    p->m_lastGroundObject = lastGroundObject;
    p->m_collidingWithLeft = collidingWithLeft;
    p->m_collidingWithRight = collidingWithRight;
    p->m_maybeSavedPlayerFrame = maybeSavedPlayerFrame;

    p->m_scaleXRelated2 = scaleXRelated2;
    p->m_groundYVelocity = groundYVelocity;
    p->m_yVelocityRelated = yVelocityRelated;
    p->m_scaleXRelated3 = scaleXRelated3;
    p->m_scaleXRelated4 = scaleXRelated4;
    p->m_scaleXRelated5 = scaleXRelated5;
    p->m_isCollidingWithSlope = isCollidingWithSlope;
    p->m_isBallRotating = isBallRotating;
    p->m_unk669 = unk669;
    p->m_currentPotentialSlope = currentPotentialSlope;
    p->m_currentSlope = currentSlope;
    p->unk_584 = unk584;
    p->m_collidingWithSlopeId = collidingWithSlopeId;
    p->m_slopeFlipGravityRelated = slopeFlipGravityRelated;

    p->m_slopeAngleRadians = slopeAngleRadians;
    p->m_rotationSpeed = rotationSpeed;
    p->m_rotateSpeed = rotateSpeed;
    p->m_isRotating = isRotating;
    p->m_isBallRotating2 = isBallRotating2;
    p->m_hasGlow = hasGlow;
    p->m_isHidden = isHidden;

    p->m_speedMultiplier = speedMultiplier;
    p->m_yStart = yStart;
    p->m_gravity = gravity;
    p->m_trailingParticleLife = trailingParticleLife;
    p->m_unk648 = unk648;
    p->m_gameModeChangedTime = gameModeChangedTime;
    p->m_padRingRelated = padRingRelated;
    p->m_maybeReducedEffects = maybeReducedEffects;
    p->m_maybeIsFalling = maybeIsFalling;
    p->m_shouldTryPlacingCheckpoint = shouldTryPlacingCheckpoint;
    p->m_playEffects = playEffects;
    p->m_maybeCanRunIntoBlocks = maybeCanRunIntoBlocks;
    p->m_hasGroundParticles = hasGroundParticles;
    p->m_hasShipParticles = hasShipParticles;
    p->m_isOnGround3 = isOnGround3;
    p->m_checkpointTimeout = checkpointTimeout;

    p->m_lastCheckpointTime = lastCheckpointTime;
    p->m_lastJumpTime = lastJumpTime;
    p->m_lastFlipTime = lastFlipTime;
    p->m_flashTime = flashTime;
    p->m_flashDuration = flashDuration;
    p->m_flashDelay = flashDelay;
    p->m_lastSpiderFlipTime = lastSpiderFlipTime;
    p->m_unkBool5 = unkBool5;
    p->m_maybeIsVehicleGlowing = maybeIsVehicleGlowing;
    p->m_switchWaveTrailColor = switchWaveTrailColor;
    p->m_practiceDeathEffect = practiceDeathEffect;

    p->m_accelerationOrSpeed = accelerationOrSpeed;
    p->m_snapDistance = snapDistance;
    p->m_ringJumpRelated = ringJumpRelated;
    p->m_objectSnappedTo = objectSnappedTo;
    p->m_pendingCheckpoint = pendingCheckpoint;
    p->m_onFlyCheckpointTries = onFlyCheckpointTries;

    p->m_jumpBuffered = jumpBuffered;
    p->m_stateRingJump = stateRingJump;
    p->m_wasJumpBuffered = wasJumpBuffered;
    p->m_wasRobotJump = wasRobotJump;
    p->m_stateJumpBuffered = stateJumpBuffered;
    p->m_stateRingJump2 = stateRingJump2;
    p->m_touchedRing = touchedRing;
    p->m_touchedCustomRing = touchedCustomRing;
    p->m_touchedGravityPortal = touchedGravityPortal;
    p->m_maybeTouchedBreakableBlock = maybeTouchedBreakableBlock;
    p->m_touchedPad = touchedPad;

    p->m_yVelocity = yVelocity;
    p->m_fallSpeed = fallSpeed;
    p->m_isOnSlope = isOnSlope;
    p->m_wasOnSlope = wasOnSlope;
    p->m_slopeVelocity = slopeVelocity;
    p->m_maybeUpsideDownSlope = maybeUpsideDownSlope;

    p->m_isShip = isShip;
    p->m_isBird = isBird;
    p->m_isBall = isBall;
    p->m_isDart = isDart;
    p->m_isRobot = isRobot;
    p->m_isSpider = isSpider;
    p->m_isUpsideDown = isUpsideDown;
    p->m_isDead = isDead;
    p->m_isOnGround = isOnGround;
    p->m_isGoingLeft = isGoingLeft;
    p->m_isSideways = isSideways;
    p->m_isSwing = isSwing;

    p->m_reverseRelated = reverseRelated;
    p->m_maybeReverseSpeed = maybeReverseSpeed;
    p->m_maybeReverseAcceleration = maybeReverseAcceleration;
    p->m_xVelocityRelated2 = xVelocityRelated2;
    p->m_isDashing = isDashing;
    p->m_groundObjectMaterial = groundObjectMaterial;
    p->m_vehicleSize = vehicleSize;
    p->m_playerSpeed = playerSpeed;
    p->m_shipRotation = shipRotation;
    p->m_lastPortalPos = lastPortalPos;
    p->m_isOnGround2 = isOnGround2;
    p->m_lastLandTime = lastLandTime;
    p->m_platformerVelocityRelated = platformerVelocityRelated;
    p->m_maybeIsBoosted = maybeIsBoosted;
    p->m_scaleXRelatedTime = scaleXRelatedTime;
    p->m_decreaseBoostSlide = decreaseBoostSlide;
    p->m_isLocked = isLocked;
    p->m_controlsDisabled = controlsDisabled;
    p->m_lastGroundedPos = lastGroundedPos;

    p->m_lastActivatedPortal = lastActivatedPortal;
    p->m_hasEverJumped = hasEverJumped;
    p->m_totalTime = totalTime;
    p->m_isSecondPlayer = isSecondPlayer;
    p->m_isBeingSpawnedByDualPortal = isBeingSpawnedByDualPortal;

    p->m_platformerXVelocity = platformerXVelocity;
    p->m_holdingRight = holdingRight;
    p->m_holdingLeft = holdingLeft;
    p->m_leftPressedFirst = leftPressedFirst;
    p->m_scaleXRelated = scaleXRelated;
    p->m_maybeHasStopped = maybeHasStopped;
    p->m_xVelocityRelated = xVelocityRelated;
    p->m_maybeGoingCorrectSlopeDirection = maybeGoingCorrectSlopeDirection;
    p->m_isSliding = isSliding;
    p->m_maybeSlopeForce = maybeSlopeForce;
    p->m_isOnIce = isOnIce;
    p->m_physDeltaRelated = physDeltaRelated;
    p->m_isOnGround4 = isOnGround4;
    p->m_maybeSlidingTime = maybeSlidingTime;
    p->m_maybeSlidingStartTime = maybeSlidingStartTime;
    p->m_changedDirectionsTime = changedDirectionsTime;
    p->m_slopeEndTime = slopeEndTime;
    p->m_isMoving = isMoving;
    p->m_platformerMovingLeft = platformerMovingLeft;
    p->m_platformerMovingRight = platformerMovingRight;
    p->m_isSlidingRight = isSlidingRight;
    p->m_maybeChangedDirectionAngle = maybeChangedDirectionAngle;
    p->m_isPlatformer = isPlatformer;

    p->m_gravityMod = gravityMod;
    p->m_affectedByForces = affectedByForces;
    p->m_fixRobotJump = fixRobotJump;
    p->m_holdingButtons = holdingButtons;
    p->m_inputsLocked = inputsLocked;
    p->m_isOutOfBounds = isOutOfBounds;
    p->m_fallStartY = fallStartY;
    p->m_ignoreDamage = ignoreDamage;
    p->m_enable22Changes = enable22Changes;
}
