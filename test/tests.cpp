// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(1);
        adapter = new DoorTimerAdapter(*door);
        timer = new Timer();
    }

    void TearDown() override {
        delete door;
        delete adapter;
        delete timer;
    }

    TimedDoor* door;
    DoorTimerAdapter* adapter;
    Timer* timer;
};

TEST_F(TimedDoorTest, initiallyDoorIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, unlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, lockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, returnsCorrectTimeout) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, adapterTimeoutThrowsWhenDoorOpen) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, adapterTimeoutNoThrowWhenDoorClosed) {
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, afterLockTimeoutDoesNothing) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, timeoutWorksAfterReopen) {
    door->unlock();
    door->lock();
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST(TimerClientMockSimpleTest, mockCanVerifyCall) {
    MockTimerClient mock;
    EXPECT_CALL(mock, Timeout()).Times(1);
    mock.Timeout();
}

TEST_F(TimedDoorTest, multipleUnlockKeepsDoorOpen) {
    door->unlock();
    door->unlock();
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, multipleLockKeepsDoorClosed) {
    door->unlock();
    door->lock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, timerTriggersTimeoutAfterDelay) {
    door->unlock();

    bool exceptionThrown = false;
    try {
        timer->tregister(door->getTimeOut(), adapter);
    } catch (const std::runtime_error& e) {
        exceptionThrown = true;
        EXPECT_STREQ(e.what(), "Door is open");
    }

    EXPECT_TRUE(exceptionThrown);
}

TEST_F(TimedDoorTest, timerDoesNotTriggerAfterDoorClosed) {
    door->unlock();
    door->lock();

    EXPECT_NO_THROW(timer->tregister(door->getTimeOut(), adapter));
}

TEST_F(TimedDoorTest, timerTriggersWithMockClient) {
    MockTimerClient mockClient;

    EXPECT_CALL(mockClient, Timeout()).Times(1);

    timer->tregister(0, &mockClient);
}

TEST_F(TimedDoorTest, timerWithZeroTimeout) {
    MockTimerClient mockClient;

    EXPECT_CALL(mockClient, Timeout()).Times(1);

    timer->tregister(0, &mockClient);
}

TEST_F(TimedDoorTest, doorUnlockTriggersTimerAndThrows) {
    door->unlock();

    EXPECT_THROW(
        timer->tregister(door->getTimeOut(), adapter),
        std::runtime_error);
}

TEST_F(TimedDoorTest, multipleTimersOnSameDoor) {
    door->unlock();

    EXPECT_THROW(
        timer->tregister(door->getTimeOut(), adapter),
        std::runtime_error);
    EXPECT_THROW(
        timer->tregister(door->getTimeOut(), adapter),
        std::runtime_error);
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, adapterHoldsReferenceToSameDoor) {
    DoorTimerAdapter adapter2(*door);
    door->unlock();
    EXPECT_THROW(adapter2.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, timeoutAfterCloseAndReopen) {
    door->unlock();
    door->lock();
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, tregisterWithNullClientDoesNothing) {
    Timer t;
    EXPECT_NO_THROW(t.tregister(1, nullptr));
}
