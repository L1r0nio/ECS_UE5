// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/2.Util/Profiler.h"

#include "DarkAnthology/ECS/Public/0.Core/Message.h"


UProfiler::UProfiler()
{
	bIsEnabled = false;
	lastFrameTime = 0;
}

void UProfiler::Reset()
{
	FScopeLock Lock(&DataLock);

	profileData.Empty();
	activeProfile.Empty();
	metrics.Empty();
	currentFrameTimes.Empty();
	lastFrameTime = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("Profiler data has been reset"));
}



void UProfiler::SetEnable(const bool bNewStatus)
{
	FScopeLock Lock(&DataLock);
	bIsEnabled = bNewStatus;
}

bool UProfiler::IsEnable() const
{
	FScopeLock Lock(&DataLock);
	return bIsEnabled;
}



void UProfiler::BeginProfile(const FString& systemName)
{
	FScopeLock Lock(&DataLock);
	if (!bIsEnabled)
		return;
	
	activeProfile.Add(systemName, FPlatformTime::Seconds());
}

void UProfiler::EndProfile(const FString& systemName, const int32 entityCount)
{
	FScopeLock Lock(&DataLock);
	if (!bIsEnabled)
		return;
	
	const double* startTime = activeProfile.Find(systemName);

	if (!startTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndProfile вызван без BeginProfile для %s"), *systemName);
		return;
	}

	float elapsedTime = static_cast<float>(FPlatformTime::Seconds() - *startTime);

	activeProfile.Remove(systemName);

	FSystemProfileData& data = profileData.FindOrAdd(systemName);
	data.SystemName = systemName;
	data.RecordSample(elapsedTime, entityCount);

	currentFrameTimes.Add(systemName, elapsedTime);

	lastFrameTime = 0.0f;

	for (const TPair<FString, float>& pair : currentFrameTimes)
		lastFrameTime += pair.Value;
}

void UProfiler::RecordMetric(const FString& metricName, const float value)
{
	if (!bIsEnabled)
		return;

	FScopeLock Lock(&DataLock);
	metrics.Add(metricName, value);
}



void UProfiler::PrintReport() const
{
	if (!bIsEnabled)
		return;
	
	FScopeLock Lock(&DataLock);

	if (profileData.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("Нет данных профилирования"));
		return;
	}

	TArray<FSystemProfileData> sortedData;
	for (const TPair<FString, FSystemProfileData>& pair : profileData)
		sortedData.Add(pair.Value);

	sortedData.Sort([](const FSystemProfileData& a, const FSystemProfileData& b)
	{
		return a.AverageTime > b.AverageTime;
	});

	float maxTime = 0.0f;
	for (const FSystemProfileData& data : sortedData)
		maxTime = FMath::Max(maxTime, data.AverageTime * 1000.0f);

	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Display, TEXT("║                                            ОТЧЕТ ПРОФИЛИРОВЩИКА ECS                                       ║"));
	UE_LOG(LogTemp, Display, TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣"));
	UE_LOG(LogTemp, Display, TEXT("║ Общее время последнего кадра: %.4f мс                                                                   ║"), lastFrameTime * 1000.0f);
	UE_LOG(LogTemp, Display, TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝"));
	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Display, TEXT("║                                    ГРАФИК ПРОИЗВОДИТЕЛЬНОСТИ СИСТЕМ (мс)                                  ║"));
	UE_LOG(LogTemp, Display, TEXT("╠═══════════════════════════════╦═══════════════════════════════════════════════════════════════════════════╣"));

	for (const FSystemProfileData& data : sortedData)
	{
		constexpr int32 graphWidth = 50;
		float avgMs = data.AverageTime * 1000.0f;
		int32 barLength = FMath::RoundToInt((avgMs / maxTime) * graphWidth);

		FString bar;
		for (int32 i = 0; i < barLength; ++i)
		{
			if (i < barLength * 0.5f)
				bar += TEXT("█");
			else if (i < barLength * 0.75f)
				bar += TEXT("▓");
			else
				bar += TEXT("▒");
		}

		UE_LOG(LogTemp, Display, TEXT("║ %-29s ║ %-*s %6.2f                 ║"),
			*data.SystemName,
			graphWidth,
			*bar,
			avgMs);
	}

	UE_LOG(LogTemp, Display, TEXT("╚═══════════════════════════════╩═══════════════════════════════════════════════════════════════════════════╝"));

	PrintPieChart(sortedData);

	UE_LOG(LogTemp, Display, TEXT(""));
	UE_LOG(LogTemp, Display, TEXT(""));
	UE_LOG(LogTemp, Display, TEXT("╔═══════════════════════════════╦══════════╦══════════╦══════════╦══════════╦══════════╦════════════════════╗"));
	UE_LOG(LogTemp, Display, TEXT("║ Система                       ║ Сред.мс  ║  Мин.мс  ║ Макс.мс  ║ Всего.мс ║ Вызовов  ║ Сред.Сущн          ║"));
	UE_LOG(LogTemp, Display, TEXT("╠═══════════════════════════════╬══════════╬══════════╬══════════╬══════════╬══════════╬════════════════════╣"));

	for (const FSystemProfileData& data : sortedData)
	{
		FString sparkline = GenerateSparkline(data);

		UE_LOG(LogTemp, Display, TEXT("║ %-29s ║ %8.4f ║ %8.4f ║ %8.4f ║ %8.2f ║ %8d ║ %18.1f ║"),
			*data.SystemName,
			data.AverageTime * 1000.0f,
			data.MinTime * 1000.0f,
			data.MaxTime * 1000.0f,
			data.TotalTime * 1000.0f,
			data.CallCount,
			data.AverageEntities);
	}

	UE_LOG(LogTemp, Display, TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝"));

	if (metrics.Num() > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗"));
		UE_LOG(LogTemp, Display, TEXT("║                                              ПОЛЬЗОВАТЕЛЬСКИЕ МЕТРИКИ                                     ║"));
		UE_LOG(LogTemp, Display, TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣"));

		for (const TPair<FString, float>& pair : metrics)
		{
			int32 barLength = FMath::Clamp(FMath::RoundToInt(pair.Value / 100.0f * 30), 0, 30);
			FString miniBar(TEXT("▓"), barLength);

			UE_LOG(LogTemp, Display, TEXT("║ ► %-40s: %12.4f  [%-30s] ║"),
				*pair.Key, pair.Value, *miniBar);
		}

		UE_LOG(LogTemp, Display, TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝"));
	}
}

void UProfiler::PrintTopSlowest(const int32 topCount) const
{
	if (!bIsEnabled)
		return;
	
	FScopeLock Lock(&DataLock);

	if (profileData.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("Нет данных профилирования"));
		return;
	}

	TArray<FSystemProfileData> sortedData;
	for (const TPair<FString, FSystemProfileData>& pair : profileData)
		sortedData.Add(pair.Value);

	sortedData.Sort([](const FSystemProfileData& a, const FSystemProfileData& b)
	{
		return a.AverageTime > b.AverageTime;
	});

	UE_LOG(LogTemp, Display, TEXT(""));
	UE_LOG(LogTemp, Display, TEXT("╔═══════════════════════════════════════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Display, TEXT("║              ТОП %2d САМЫХ МЕДЛЕННЫХ СИСТЕМ                             ║"), topCount);
	UE_LOG(LogTemp, Display, TEXT("╠═══╦═══════════════════════════════════╦══════════╦══════════╦══════════╣"));
	UE_LOG(LogTemp, Display, TEXT("║ № ║ Система                           ║ Сред.мс  ║  Мин.мс  ║ Макс.мс  ║"));
	UE_LOG(LogTemp, Display, TEXT("╠═══╬═══════════════════════════════════╬══════════╬══════════╬══════════╣"));

	const int32 count = FMath::Min(topCount, sortedData.Num());

	for (int32 i = 0; i < count; ++i)
	{
		const FSystemProfileData& data = sortedData[i];
		const TCHAR* medal = (i == 0) ? TEXT("█") : (i == 1) ? TEXT("▓") : (i == 2) ? TEXT("▒") : TEXT("░");

		UE_LOG(LogTemp, Display, TEXT("║%s%2d ║ %-33s ║ %8.4f ║ %8.4f ║ %8.4f ║"),
			medal,
			i + 1,
			*data.SystemName,
			data.AverageTime * 1000.0f,
			data.MinTime * 1000.0f,
			data.MaxTime * 1000.0f);
	}

	UE_LOG(LogTemp, Display, TEXT("╚═══╩═══════════════════════════════════╩══════════╩══════════╩══════════╝"));
	UE_LOG(LogTemp, Display, TEXT(""));
}

void UProfiler::SaveReport(const FString& filePath) const
{
	FScopeLock Lock(&DataLock);
	if (!bIsEnabled)
		return;
	
	FString report;

	TArray<FSystemProfileData> sortedData;

	for (const TPair<FString, FSystemProfileData>& Pair : profileData)
		sortedData.Add(Pair.Value);

	sortedData.Sort([](const FSystemProfileData& a, const FSystemProfileData& b)
	{
		return a.AverageTime > b.AverageTime;
	});

	report += TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
	report += TEXT("║                                          ECS PROFILER REPORT                                              ║\n");
	report += TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
	report += FString::Printf(TEXT("║ Дата: %-99s ║\n"), *FDateTime::Now().ToString());
	report += FString::Printf(TEXT("║ Общее время кадра: %8.4f мс                                                                            ║\n"), lastFrameTime * 1000.0f);
	report += FString::Printf(TEXT("║ Количество систем: %8d                                                                               ║\n"), profileData.Num());
	report += TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");


	float maxTime = 0.0f;
	for (const FSystemProfileData& data : sortedData)
		maxTime = FMath::Max(maxTime, data.AverageTime * 1000.0f);

	report += TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
	report += TEXT("║                                    ГРАФИК ПРОИЗВОДИТЕЛЬНОСТИ СИСТЕМ (мс)                                  ║\n");
	report += TEXT("╠═══════════════════════════════╦═══════════════════════════════════════════════════════════════════════════╣\n");

	const int32 graphWidth = 50;
	for (const FSystemProfileData& data : sortedData)
	{
		float avgMs = data.AverageTime * 1000.0f;
		int32 barLength = FMath::RoundToInt((avgMs / maxTime) * graphWidth);

		FString bar;
		for (int32 i = 0; i < barLength; ++i)
		{
			if (i < barLength * 0.5f)
				bar += TEXT("█");
			else if (i < barLength * 0.75f)
				bar += TEXT("▓");
			else
				bar += TEXT("▒");
		}

		report += FString::Printf(TEXT("║ %-29s ║ %-*s %6.2f                 ║\n"),
			*data.SystemName,
			graphWidth,
			*bar,
			avgMs);
	}

	report += TEXT("╚═══════════════════════════════╩═══════════════════════════════════════════════════════════════════════════╝\n\n");


	report += TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
	report += TEXT("║                                        РАСПРЕДЕЛЕНИЕ ВРЕМЕНИ                                              ║\n");
	report += TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");

	float totalTime = 0.0f;
	for (const FSystemProfileData& data : sortedData)
		totalTime += data.AverageTime;

	const TCHAR* symbols[] = { TEXT("█"), TEXT("▓"), TEXT("▒"), TEXT("░"), TEXT("·") };

	float accumulatedPercent = 0.0f;

	for (int32 i = 0; i < FMath::Min(5, sortedData.Num()); ++i)
	{
		float percent = (sortedData[i].AverageTime / totalTime) * 100.0f;
		accumulatedPercent += percent;
		int32 barLength = FMath::RoundToInt(percent / 2.0f);

		FString bar;
		for (int32 j = 0; j < barLength; ++j)
			bar += symbols[i];

		report += FString::Printf(TEXT("║ %s %-25s %s%-*s %5.1f%%                    ║\n"),
			symbols[i],
			*sortedData[i].SystemName,
			symbols[i],
			50,
			*bar,
			percent);
	}

	if (sortedData.Num() > 5)
	{
		report += FString::Printf(TEXT("║   %-25s   %-50s %5.1f%%                    ║\n"),
			TEXT("Остальные"),
			TEXT(""),
			100.0f - accumulatedPercent);
	}

	report += TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");


	report += TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
	report += TEXT("║                                    СРАВНЕНИЕ MIN/AVG/MAX (Топ-5 систем)                                   ║\n");
	report += TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");

	for (int32 i = 0; i < FMath::Min(5, sortedData.Num()); ++i)
	{
		const FSystemProfileData& data = sortedData[i];
		float minMs = data.MinTime * 1000.0f;
		float avgMs = data.AverageTime * 1000.0f;
		float maxMs = data.MaxTime * 1000.0f;

		int32 minBar = FMath::RoundToInt((minMs / maxMs) * 40);
		int32 avgBar = FMath::RoundToInt((avgMs / maxMs) * 40);

		FString minLine(TEXT("░"), minBar);
		FString avgLine(TEXT("▓"), avgBar);
		FString maxLine(TEXT("█"), 40);

		report += FString::Printf(TEXT("║ %-29s ║ Min: %-40s %6.2f                      ║\n"), 
			*data.SystemName, *minLine, minMs);
		report += FString::Printf(TEXT("║ %-29s ║ Avg: %-40s %6.2f                      ║\n"), 
			TEXT(""), *avgLine, avgMs);
		report += FString::Printf(TEXT("║ %-29s ║ Max: %-40s %6.2f                      ║\n"), 
			TEXT(""), *maxLine, maxMs);

		if (i < FMath::Min(4, sortedData.Num() - 1))
			report += TEXT("║                               ╟───────────────────────────────────────────────────────────────────────────╢\n");
	}

	report += TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");


	report += TEXT("╔═══════════════════════════════╦══════════╦══════════╦══════════╦══════════╦══════════╦════════════════════╗\n");
	report += TEXT("║ Система                       ║ Сред.мс  ║  Мин.мс  ║ Макс.мс  ║ Всего.мс ║ Вызовов  ║ Сред.Сущн          ║\n");
	report += TEXT("╠═══════════════════════════════╬══════════╬══════════╬══════════╬══════════╬══════════╬════════════════════╣\n");

	for (const FSystemProfileData& data : sortedData)
	{
		FString sparkline = GenerateSparkline(data);

		report += FString::Printf(TEXT("║ %-29s ║ %8.4f ║ %8.4f ║ %8.4f ║ %8.2f ║ %8d ║ %18.1f ║\n"),
			*data.SystemName,
			data.AverageTime * 1000.0f,
			data.MinTime * 1000.0f,
			data.MaxTime * 1000.0f,
			data.TotalTime * 1000.0f,
			data.CallCount,
			data.AverageEntities);
	}

	report += TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");

	if (metrics.Num() > 0)
	{
		report += TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
		report += TEXT("║                                              ПОЛЬЗОВАТЕЛЬСКИЕ МЕТРИКИ                                     ║\n");
		report += TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");

		for (const TPair<FString, float>& pair : metrics)
		{
			int32 barLength = FMath::Clamp(FMath::RoundToInt(pair.Value / 100.0f * 30), 0, 30);
			FString miniBar(TEXT("▓"), barLength);

			report += FString::Printf(TEXT("║ ► %-40s: %12.4f  [%-30s] ║\n"), 
				*pair.Key, pair.Value, *miniBar);
		}

		report += TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
	}
	
	const bool bSuccess = FFileHelper::SaveStringToFile(report, *filePath);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("✓ Отчет профилировщика сохранен в %s"), *filePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("✗ Не удалось сохранить отчет в %s"), *filePath);
	}
}



FString UProfiler::GenerateSparkline(const FSystemProfileData& data) const
{
	const TCHAR* sparkChars[] = { TEXT("▁"), TEXT("▂"), TEXT("▃"), TEXT("▄"), TEXT("▅"), TEXT("▆"), TEXT("▇"), TEXT("█") };

	FString sparkline;

	float minVal = data.MinTime;
	float maxVal = data.MaxTime;

	for (float val : data.TimeHistory)
	{
		float normalized = (val - minVal) / (maxVal - minVal + 0.0001f);
		int32 index = FMath::Clamp(FMath::RoundToInt(normalized * 7), 0, 7);
		sparkline += sparkChars[index];
	}

	return sparkline;
}

FString UProfiler::GenerateMiniGraph(const TArray<float>& values, int32 samples) const
{
	const TCHAR* bars[] = { TEXT("▁"), TEXT("▂"), TEXT("▃"), TEXT("▄"), TEXT("▅"), TEXT("▆"), TEXT("▇"), TEXT("█") };
	FString graph;

	int32 start = FMath::Max(0, values.Num() - samples);
	float minVal = FLT_MAX, maxVal = 0.0f;

	for (int32 i = start; i < values.Num(); ++i)
	{
		minVal = FMath::Min(minVal, values[i]);
		maxVal = FMath::Max(maxVal, values[i]);
	}

	for (int32 i = start; i < values.Num(); ++i)
	{
		float normalized = (values[i] - minVal) / (maxVal - minVal + 0.0001f);
		int32 barIndex = FMath::Clamp(FMath::FloorToInt(normalized * 7), 0, 7);
		graph += bars[barIndex];
	}

	return graph;
}

void UProfiler::PrintPieChart(const TArray<FSystemProfileData>& sortedData) const
{
	if (!bIsEnabled)
		return;
	
	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Display, TEXT("║                                             РАСПРЕДЕЛЕНИЕ ВРЕМЕНИ                                         ║"));
	UE_LOG(LogTemp, Display, TEXT("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣"));

	float totalTime = 0.0f;
	for (const FSystemProfileData& data : sortedData)
		totalTime += data.AverageTime;

	const TCHAR* symbols[] = { TEXT("█"), TEXT("▓"), TEXT("▒"), TEXT("░"), TEXT("·") };

	for (int32 i = 0; i < FMath::Min(5, sortedData.Num()); ++i)
	{
		float percent = (sortedData[i].AverageTime / totalTime) * 100.0f;
		int32 barLength = FMath::RoundToInt(percent / 2.0f);

		FString bar;
		for (int32 j = 0; j < barLength; ++j)
			bar += symbols[i];

		UE_LOG(LogTemp, Display, TEXT("║ %s %-25s %s%-*s %5.1f%%                    ║"),
			symbols[i],
			*sortedData[i].SystemName,
			symbols[i],
			50,
			*bar,
			percent);
	}

	UE_LOG(LogTemp, Display, TEXT("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝"));
	UE_LOG(LogTemp, Display, TEXT(" "));
	UE_LOG(LogTemp, Display, TEXT(" "));
}

void UProfiler::DrawRealtimeStats(const UWorld* World) const
{
	if (!bIsEnabled)
		return;
	
	if (!GEngine || !World)
		return;

	FScopeLock Lock(&DataLock);

	int32 Y = 70;
	const int32 LineHeight = 15;

	FColor FrameColor = lastFrameTime > 0.016f ? FColor::Red : FColor::Green;

	DrawDebugString(World, FVector(0, 0, 80), 
		FString::Printf(TEXT("Frame time: %.5f ms"), lastFrameTime * 1000.0f),
		nullptr, FrameColor, 0.0f, true, 1.5f);
	Y += LineHeight * 2;

	TArray<FSystemProfileData> sortedData;
	profileData.GenerateValueArray(sortedData);

	sortedData.Sort([](const FSystemProfileData& a, const FSystemProfileData& b)
	{
		return a.AverageTime > b.AverageTime;
	});

	for (int32 i = 0; i < FMath::Min(3, sortedData.Num()); ++i)
	{
		const FSystemProfileData& data = sortedData[i];

		FColor color = FColor::Green;
		if (data.ActiveWarnings.Num() > 0)
			color = FColor::Yellow;
		if (data.AverageTime > 0.008f)
			color = FColor::Red;

		FString miniGraph = GenerateMiniGraph(data.TimeHistory, 10);

		DrawDebugString(World, FVector(0, 0, 80 - Y),
			FString::Printf(TEXT("%-15s: %.5f ms %s"), *data.SystemName.Left(15), data.AverageTime * 1000.0f, *miniGraph),
			nullptr, color, 0.0f, true, 1.0f);

		Y += LineHeight;
	}
}


const FSystemProfileData* UProfiler::GetSystemData(const FString& systemName) const
{
	FScopeLock Lock(&DataLock);
	return profileData.Find(systemName);
}

const TMap<FString, FSystemProfileData>& UProfiler::GetAllData() const
{
	return profileData;
}

float UProfiler::GetLastFrameTime() const
{
	FScopeLock Lock(&DataLock);
	return lastFrameTime;
}
