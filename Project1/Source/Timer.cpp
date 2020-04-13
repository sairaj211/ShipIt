#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Timer.h"

Timer* Timer::mInstance = nullptr;

Timer::Timer()
{
	
}

Timer::~Timer()
{
	
}

Timer* Timer::GetInstance()
{
	if (!mInstance)
		mInstance = new Timer();

	return mInstance;
}

void Timer::Draw()
{
	ImGui::Begin("Profiler");
	for (int i = 0; i < mRecords.size(); ++i) {
		Record record = mRecords[i];

		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%s: %.2f ms", record.message.c_str(), record.duration);
		ImGui::Text(buffer);
	}
	mRecords.clear();
	ImGui::End();
}

void Timer::Start(std::string a_message)
{
	Record record;
	record.message = a_message;
	record.start = std::chrono::high_resolution_clock::now();
	record.duration = 0.0f;
	mActiveStack.push(int(mRecords.size()));

	mRecords.push_back(record);
}

void Timer::End()
{
	int activeIndex = mActiveStack.top();
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - mRecords[activeIndex].start;
	mRecords[activeIndex].duration = duration.count() * 1000.0f;
	mActiveStack.pop();
}


