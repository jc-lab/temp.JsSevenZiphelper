#include <iostream>
#include <string>
#include <list>

#include "JsSevenZipHelper/JsSevenZipHelper.h"

int test_compress(int argc, char *argv[])
{
	JsSevenZipHelper::Compress compress;
	std::list<JsSevenZipHelper::Compress::FileListItem> fileList;
	JsSevenZipHelper::FileOutStream *pOutStreamSpec = new JsSevenZipHelper::FileOutStream();
	CMyComPtr<JsSevenZipHelper::FileOutStream> pOutStream = pOutStreamSpec; // ComPtr을 이용하기 때문에 따로 pOutStreamSpec을 delete할 필요 없음.

	pOutStreamSpec->Create("D:\\test.7z", true); // 저장할 파일을 생성한다.
	
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(true,  false, std::string("D:\\folder"), std::string("folder"))); // 원래는 되야 하는데... 안된다.. 아직 이유를 찾고 있는 중이다.
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(false, false, std::string("D:\\test1.txt"), std::string("test1.txt"))); // 앞부분은 실제 파일 경로, 뒷부분은 Archive내 저장할 경로
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(false, false, std::string("D:\\test2.bin"), std::string("1/test.bin"))); // 이런건 자동으로 디렉터리가 생성된다.
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(false, false, std::string("D:\\test2.bin"), std::string("2/test.bin"))); // 이런건 자동으로 디렉터리가 생성된다.
	
	compress.createArchive(&CLSID_CFormat7z); // 7z 형식의 Archive을 생성한다.

	// compress(ISequentialOutStream *pOutStreamSpec, const std::list<FileListItem>& fileList, bool isDefinedPassword, bool isHeaderEncrypt, std::string strPassword)
	// compress(출력스트림, 파일리스트, 비밀번호 설정할 것인지, 헤더까지 암호화할 것인지, 비밀번호)
	compress.compress(pOutStream, fileList, true, true, "test1234");

	return 0;
}

int test_extract(int argc, char *argv[])
{
	JsSevenZipHelper::Extract extract;
	std::list<JsSevenZipHelper::Extract::FileListItem> extractFileList;
	JsSevenZipHelper::FileInStream *pInStreamSpec = new JsSevenZipHelper::FileInStream();
	CMyComPtr<JsSevenZipHelper::FileInStream> pInStream = pInStreamSpec; // ComPtr을 이용하기 때문에 따로 pInStreamSpec을 delete할 필요 없음.

	pInStreamSpec->Open("D:\\test.7z"); // 압축 파일을 연다.

	// openArchive(const GUID *pFormatGUID, IInStream *pInStream, bool isDefinedPassword, std::string strPassword)
	// openArchive(파일 형식, 입력스트림, 비밀번호가 설정되어있는지, 비밀번호)
	extract.openArchive(&CLSID_CFormat7z, pInStream, true, "test1234");
	
	// extract(const std::string& strExtractRootPath, IArchiveExtractCallback *extractCallback, std::list<FileListItem> *pFileList)
	// extract(압축 풀 디렉터리, 사용자 정의 IArchiveExtractCallback을 사용할 경우, 압축 푼 파일 리스트)
	extract.extract("D:\\extracttest", NULL, &extractFileList);

	return 0;
}

int main(int argc, char *argv[])
{
	test_compress(argc, argv);
	test_extract(argc, argv);
}
