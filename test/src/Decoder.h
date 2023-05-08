#ifndef __DECODER_H__
#define __DECODER_H__

//==============================================================================
// generated decoder code below
//==============================================================================

#include <algorithm>
#include <string>
#include "Base64.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

//#define TEST_UNIQUE_PTR_DELETE
extern const char gPassword[];

//==============================================================================
// the stream cipher
//==============================================================================
class CCXR
{
protected:
	CCXR(const BYTE* key, unsigned int ks)
	{
		int i; BYTE rs; unsigned kp;
		for (i = 0; i < 256; i++)c[i] = i; kp = 0; rs = 0; for (i = 255; i; i--)std::swap(c[i], c[kr(i, key, ks, &rs, &kp)]); r2 = c[1]; r1 = c[3]; av = c[5]; lp = c[7]; lc = c[rs]; rs = 0; kp = 0;
	}
	inline void SC() { BYTE st = c[lc]; r1 += c[r2++]; c[lc] = c[r1]; c[r1] = c[lp]; c[lp] = c[r2]; c[r2] = st; av += c[st]; }
	BYTE c[256], r2, r1, av, lp, lc;

	BYTE kr(unsigned int lm, const BYTE* uk, BYTE ks, BYTE* rs, unsigned* kp)
	{
		unsigned rl = 0, mk = 1, u; while (mk < lm)mk = (mk << 1) + 1; do { *rs = c[*rs] + uk[(*kp)++]; if (*kp >= ks) { *kp = 0; *rs += ks; }u = mk & *rs; if (++rl > 11)u %= lm; } while (u > lm); return u;
	}
};

struct CXRD :CCXR
{
	CXRD(const BYTE* userKey, unsigned int keyLength = 16) : CCXR(userKey, keyLength) {}
	inline BYTE pb(BYTE b) { SC(); lp = b ^ c[(c[r1] + c[r2]) & 0xFF] ^ c[c[(c[lp] + c[lc] + c[av]) & 0xFF]]; lc = b; return lp; }
};


//==============================================================================
// the high-level decoding function
//==============================================================================
std::unique_ptr<char[]> __CXRDecrypt(const char* in, const char* pw)
{
	std::vector<BYTE> db = base64_decode(in);
	std::string is(reinterpret_cast<const char*>(&db[0]), db.size());
	std::vector<BYTE> bos;
	CXRD sap((const BYTE*)pw, strlen(pw));
	for (unsigned int i = 0; i < is.length(); i++) {
		char c = (char)(sap.pb((BYTE)(is.at(i))));
		if (i > 0) { bos.push_back(c); }
	}
	auto ptr = std::make_unique<char[]>(255);
	std::copy(bos.begin(), bos.end(), ptr.get());
	return ptr;
}



#ifdef TEST_UNIQUE_PTR_DELETE
class state_deleter {  // a deleter class with state
	int count_;
public:
	state_deleter() : count_(0) {}
	template <class T>
	void operator()(T* p) {
		std::cout << "[deleted #" << ++count_ << "]\n";
		delete p;
	}
};
#endif
namespace {
	class Resource {
	public:
		struct Parameters {
			std::size_t size_bytes = 0;
			void* ptr = nullptr;
		};
	private:
		HRSRC hResource = nullptr;
		HGLOBAL hMemory = nullptr;

		Parameters p;

	public:
		Resource(int resource_id, const std::string& resource_class) {
			hResource = FindResourceA(nullptr, MAKEINTRESOURCEA(resource_id), resource_class.c_str());
			hMemory = LoadResource(nullptr, hResource);

			p.size_bytes = SizeofResource(nullptr, hResource);
			p.ptr = LockResource(hMemory);
		}

		auto& GetResource() const {
			return p;
		}

		auto GetResourceString() const {
			std::string_view dst;
			if (p.ptr != nullptr)
				dst = std::string_view(reinterpret_cast<char*>(p.ptr), p.size_bytes);
			return dst;
		}
	};

	void GetFile() {
		Resource very_important(IDR_TEXT1, "TEXT");
		auto dst = very_important.GetResourceString();

		Resource more_data(IDR_TEXT2, "TEXT");
		auto dst_2 = more_data.GetResourceString();
	}
}

#ifdef TEST_UNIQUE_PTR_DELETE
std::unique_ptr<char, state_deleter> GetResString(int id) {
	auto ptr = std::unique_ptr<char, state_deleter>{ new char[255]() };
	LoadString(nullptr, id, ptr.get(), 255);
	return ptr;
}
#else
std::unique_ptr<char> GetResString(int id) {
	auto ptr = std::unique_ptr<char>{ new char[255]() };
	LoadString(nullptr, id, ptr.get(), 255);
	return ptr;
}
#endif

#define RES_STRING(x)   GetResString(x)

const char* test_get_res_string_decrypted(int id, const char* p)
{
	auto ptr = RES_STRING(id);
	CString dec = Decrypt(ptr.get(), p);
	return dec.GetBuffer();
}

std::unique_ptr<char[]> get_res_string_decrypted(int id, const char* p)
{
	auto ptr = RES_STRING(id);
	return __CXRDecrypt(ptr.get(), p);
}

#define DECRYPT_RES(x)   get_res_string_decrypted(x,gPassword)

#endif // __DECODER_H__