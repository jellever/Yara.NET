#pragma once
namespace YaraNET
{
	public ref class YaraString
	{
	private:
		IntPtr memoryAddress;
		IntPtr offset;
		array<Byte>^ data;
		String^ identifierName;

	public:
		YaraString(void* dataPtr, IntPtr dataSize, IntPtr offset, IntPtr memoryAddress, String^ identifier);

		property String^ IdentifierName {
			String^ get() {
				return this->identifierName;
			}
		}

		property IntPtr MemoryAddress {
			IntPtr get() {
				return this->memoryAddress;
			}
		}

		property IntPtr Offset {
			IntPtr get() {
				return this->offset;
			}
		}

		property array<Byte>^ Data {
			array<Byte>^ get() {
				return this->data;
			}
		}
	};
}

