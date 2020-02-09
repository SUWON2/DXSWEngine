#pragma once

// 함수의 제일 첫 번째 매개변수를 CoreKey로 지정함으로써 Core 클래스 구간 외에 함수 호출를 금지시킬 수 있습니다.
// 제일 첫 번째 파라미터를 CoreKey로 지정한 함수는 접두어 _를 붙입니다.
class CoreKey final
{
private:
	friend class Core;

private:
	CoreKey() = delete;

	CoreKey(const CoreKey&) = default;

	CoreKey& operator=(const CoreKey&) = delete;

	~CoreKey() = default;
};