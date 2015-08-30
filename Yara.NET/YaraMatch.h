#pragma once

namespace YaraNET
{
	public ref class YaraMatch
	{
	private:
		List<YaraString^>^ matchData;
		YaraRule^ rule;
	public:
		YaraMatch(YaraRule^ rule);

		property bool ContainsMatches {
			bool get()
			{
				return this->matchData->Count > 0;
			}
		}

		property List<YaraString^>^ MatchData {
			List<YaraString^>^ get()
			{
				return gcnew List<YaraString^>(this->matchData);
			}
		}

		property YaraRule^ Rule {
			YaraRule^ get()
			{
				return this->rule;
			}
		}

		void AddMatch(YaraString^ yaraString);
	};
}
