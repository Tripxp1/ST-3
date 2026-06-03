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

// Mock Door for testing
class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(1);  // 1 second timeout for faster tests
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

// Existing tests
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

// New tests for Timer functionality
TEST_F(TimedDoorTest, timerTriggersTimeoutAfterDelay) {
    door->unlock();
<<<<<<< HEAD

    bool exceptionThrown = false;
    try {
        timer->tregister(door->getTimeOut(), adapter);
=======
    
    bool exceptionThrown = false;
    try {
        timer->tregister(door->getTimeOut(), adapter);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
>>>>>>> 150a37e70d9ad1d5ebae54f33fd312b67532c2e0
    } catch (const std::runtime_error& e) {
        exceptionThrown = true;
        EXPECT_STREQ(e.what(), "Door is open");
    }
<<<<<<< HEAD

=======
    
>>>>>>> 150a37e70d9ad1d5ebae54f33fd312b67532c2e0
    EXPECT_TRUE(exceptionThrown);
}

TEST_F(TimedDoorTest, timerDoesNotTriggerAfterDoorClosed) {
    door->unlock();
<<<<<<< HEAD
    door->lock();

    EXPECT_NO_THROW(timer->tregister(door->getTimeOut(), adapter));
=======
    door->lock();  // Close door before timeout
    
    bool exceptionThrown = false;
    try {
        timer->tregister(door->getTimeOut(), adapter);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    } catch (const std::runtime_error& e) {
        exceptionThrown = true;
    }
    
    EXPECT_FALSE(exceptionThrown);
>>>>>>> 150a37e70d9ad1d5ebae54f33fd312b67532c2e0
}

TEST_F(TimedDoorTest, timerTriggersWithMockClient) {
    MockTimerClient mockClient;
    
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    
    timer->tregister(0, &mockClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(TimedDoorTest, timerWithZeroTimeout) {
    MockTimerClient mockClient;
    
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    
    timer->tregister(0, &mockClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(TimedDoorTest, doorUnlockTriggersTimerAndThrows) {
    door->unlock();
<<<<<<< HEAD

    EXPECT_THROW(timer->tregister(door->getTimeOut(), adapter), std::runtime_error);
=======
    
    EXPECT_THROW({
        timer->tregister(door->getTimeOut(), adapter);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }, std::runtime_error);
>>>>>>> 150a37e70d9ad1d5ebae54f33fd312b67532c2e0
}

TEST_F(TimedDoorTest, multipleTimersOnSameDoor) {
    door->unlock();
<<<<<<< HEAD

    EXPECT_THROW(timer->tregister(door->getTimeOut(), adapter), std::runtime_error);
    EXPECT_THROW(timer->tregister(door->getTimeOut(), adapter), std::runtime_error);
=======
    
    timer->tregister(door->getTimeOut(), adapter);
    timer->tregister(door->getTimeOut(), adapter);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    
>>>>>>> 150a37e70d9ad1d5ebae54f33fd312b67532c2e0
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
