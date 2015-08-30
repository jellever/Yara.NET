#pragma once
namespace YaraNET
{
	ref class YaraScanCallBackData
	{
	private:
		List<YaraMatch^>^ matches;
		IntPtr dataBaseAddress;
	public:

		property List<YaraMatch^>^ Matches {
			List<YaraMatch^>^ get() {
				return gcnew List<YaraMatch^>(this->matches);
			}
		}

		property IntPtr DataBaseAddress {
			IntPtr get() {
				return this->dataBaseAddress;
			}
		}

		YaraScanCallBackData(IntPtr dataBaseAddress);
		void AddYaraMatch(YaraMatch^ yaraMatch);
	};
}