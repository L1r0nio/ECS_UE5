// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Profiler.generated.h"



UENUM()
enum class EPerformanceWarning : uint8
{
	None,
	SlowFrame,
	HighVariance,
	Regression,
	EntityInefficient
};



USTRUCT()
struct FSystemProfileData
{
	GENERATED_BODY()
	
	FString SystemName;

	float TotalTime = 0.0f;
	float AverageTime = 0.0f;
	float MinTime = FLT_MAX;
	float MaxTime = 0.0f;

	int32 CallCount = 0;

	int32 TotalEntities = 0;
	float AverageEntities = 0.0f;
	
	static constexpr int32 HISTORY_SIZE = 120;
	TArray<float> TimeHistory;
	TArray<int32> EntityHistory;
	
	float Percentile95 = 0.0f;
	float StandardDeviation = 0.0f;   
	float TimePerEntity = 0.0f;  

	TArray<EPerformanceWarning> ActiveWarnings;

	FSystemProfileData()
	{
		TimeHistory.Reserve(HISTORY_SIZE);
		EntityHistory.Reserve(HISTORY_SIZE);
	}

	void RecordSample(const float time, const int32 entityCount)
	{
		TotalTime += time;
		CallCount++;
		AverageTime = TotalTime / CallCount;

		MinTime = FMath::Min(MinTime, time);
		MaxTime = FMath::Max(MaxTime, time);

		TotalEntities += entityCount;
		AverageEntities = static_cast<float>(TotalEntities) / CallCount;


		if (TimeHistory.Num() >= HISTORY_SIZE)
		{
			TimeHistory.RemoveAt(0);
			EntityHistory.RemoveAt(0);
		}
		
		TimeHistory.Add(time);
		EntityHistory.Add(entityCount);

		UpdatePercentilesAndStats();

		TimePerEntity = entityCount > 0 ? time / entityCount : 0.0f;

		AnalyzePerformance();
	}

	void Reset()
	{
		TotalTime = 0.0f;
		AverageTime = 0.0f;
		MinTime = FLT_MAX;
		MaxTime = 0.0f;
		CallCount = 0;
		TotalEntities = 0;
		AverageEntities = 0.0f;

		TimeHistory.Reset();
		EntityHistory.Reset();

		Percentile95 = 0.0f;
		StandardDeviation = 0.0f;
		TimePerEntity = 0.0f;

		ActiveWarnings.Empty();
	}

private:
	void UpdatePercentilesAndStats()
	{
		if (TimeHistory.Num() < 10)
		{
			Percentile95 = AverageTime;
			StandardDeviation = 0.0f;
			return;
		}

		TArray<float> sorted = TimeHistory;
		sorted.Sort();

		const int32 index95 = FMath::Clamp(FMath::FloorToInt(sorted.Num() * 0.95f), 0, sorted.Num() - 1);
		Percentile95 = sorted[index95];
		
		float variance = 0.0f;
		for (float Val : sorted)
		{
			float Diff = Val - AverageTime;
			variance += Diff * Diff;
		}
		
		StandardDeviation = FMath::Sqrt(variance / sorted.Num());
	}

	void AnalyzePerformance()
	{
		ActiveWarnings.Empty();

		if (AverageTime > 0.016f)
			ActiveWarnings.Add(EPerformanceWarning::SlowFrame);

		if (StandardDeviation > AverageTime * 0.5f)
			ActiveWarnings.Add(EPerformanceWarning::HighVariance);

		if (TimePerEntity > 0.001f && AverageEntities > 100.0f) 
			ActiveWarnings.Add(EPerformanceWarning::EntityInefficient);
	}
};


UCLASS()
class DARKANTHOLOGY_API UProfiler : public UObject
{
	GENERATED_BODY()

public:
	UProfiler();
	virtual ~UProfiler() override = default;

	void Reset();
	void SetEnable(const bool bNewStatus);
	bool IsEnable() const;

	void BeginProfile(const FString& systemName);
	void EndProfile(const FString& systemName, const int32 entityCount = 0);
	void RecordMetric(const FString& metricName, const float value);

	void PrintReport() const;
	void PrintTopSlowest(const int32 topCount = 5) const;
	void SaveReport(const FString& filePath) const;

	const FSystemProfileData* GetSystemData(const FString& systemName) const;
	const TMap<FString, FSystemProfileData>& GetAllData() const;
	float GetLastFrameTime() const;
	
	void DrawRealtimeStats(const UWorld* World) const;

private:
	UPROPERTY(VisibleAnywhere)
	TMap<FString, FSystemProfileData> profileData;

	UPROPERTY(VisibleAnywhere)
	TMap<FString, double> activeProfile;

	UPROPERTY(VisibleAnywhere)
	TMap<FString, float> metrics;

	UPROPERTY(VisibleAnywhere)
	TMap<FString, float> currentFrameTimes;

	mutable FCriticalSection DataLock;

	bool bIsEnabled;
	float lastFrameTime;

	FString GenerateSparkline(const FSystemProfileData& data) const;
	FString GenerateMiniGraph(const TArray<float>& values, int32 samples) const;
	void PrintPieChart(const TArray<FSystemProfileData>& sortedData) const;

};

class FProfileScope
{
public:
	FProfileScope(UProfiler* inProfiler, const FString& inScopeName, const int32 inEntityCount = 0)
		: profiler(inProfiler), scopeName(inScopeName), entityCount(inEntityCount)
	{
		if (profiler && profiler->IsEnable())
			profiler->BeginProfile(scopeName);
	}

	~FProfileScope()
	{
		if (profiler && profiler->IsEnable())
			profiler->EndProfile(scopeName, entityCount);
	}

	void SetEntityCount(const int32 count)
	{
		entityCount = count;
	}

private:
	UProfiler* profiler;
	FString scopeName;
	int32 entityCount;
};


#define PROFILE_ECS_SCOPE(Profiler, Name) \
	FProfileScope _ProfileScope(Profiler, Name)

#define PROFILE_ECS_SCOPE_WITH_COUNT(Profiler, Name, Count) \
	FProfileScope _ProfileScope(Profiler, Name, Count)