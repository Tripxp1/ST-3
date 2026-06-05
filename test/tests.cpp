// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "TimedDoor.h"

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = std::make_unique<TimedDoor>(kDefaultTimeoutMs);
  }

  void TearDown() override {
    door.reset();
  }

  static constexpr int kDefaultTimeoutMs = 5;
  std::unique_ptr<TimedDoor> door;
};

TEST_F(TimedDoorTest, DoorClosedAfterConstruction) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsConstructorValue) {
  TimedDoor customDoor(42);
  EXPECT_EQ(42, customDoor.getTimeOut());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  std::thread unlockThread([this]() {
    try {
      door->unlock();
    } catch (const std::runtime_error&) {
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  door->lock();
  unlockThread.join();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoorDuringTimerWait) {
  std::thread unlockThread([this]() {
    try {
      door->unlock();
    } catch (const std::runtime_error&) {
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  unlockThread.join();
}

TEST_F(TimedDoorTest, IsDoorOpenedTrueWhileOpenBeforeTimeout) {
  std::atomic<bool> timerRunning{false};
  std::thread unlockThread([this, &timerRunning]() {
    timerRunning = true;
    try {
      door->unlock();
    } catch (const std::runtime_error&) {
    }
    timerRunning = false;
  });
  while (!timerRunning) {
    std::this_thread::yield();
  }
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  unlockThread.join();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorOpen) {
  TimedDoor openDoor(0);
  EXPECT_THROW(openDoor.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, UnlockDoesNotThrowWhenClosedBeforeTimeout) {
  std::thread closer([this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    door->lock();
  });
  EXPECT_NO_THROW(door->unlock());
  closer.join();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimerAdapterTimeoutThrowsWhenDoorStaysOpen) {
  TimedDoor shortDoor(1);
  EXPECT_THROW(shortDoor.unlock(), std::runtime_error);
  EXPECT_TRUE(shortDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, TimerTregisterStoresClientAndCompletes) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_NO_THROW(timer.tregister(0, &mockClient));
}

TEST_F(TimedDoorTest, MockDoorInterface) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, lock()).Times(1);
  EXPECT_CALL(mockDoor, unlock()).Times(1);
  EXPECT_CALL(mockDoor, isDoorOpened())
      .WillOnce(::testing::Return(false))
      .WillOnce(::testing::Return(true));

  mockDoor.lock();
  mockDoor.unlock();
  EXPECT_FALSE(mockDoor.isDoorOpened());
  EXPECT_TRUE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, MockTimerClientInterface) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  mockClient.Timeout();
}
