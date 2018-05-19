#pragma once

#include "Base.h"
#include <optional>

namespace cru {
	class HResultError : public std::runtime_error
	{
	public:
		HResultError(HRESULT h_result);
		HResultError(HRESULT h_result, const std::string& message);
		~HResultError() override;

		HRESULT GetHResult() const;

	private:
		std::string MakeMessage(HRESULT h_result, std::optional<std::string> message);

	private:
		HRESULT h_result_;
	};

	inline void ThrowIfFailed(HRESULT h_result) {
		if (FAILED(h_result))
			throw HResultError(h_result);
	}

	inline void ThrowIfFailed(HRESULT h_result, const std::string& message) {
		if (FAILED(h_result))
			throw HResultError(h_result, message);
	}
}
