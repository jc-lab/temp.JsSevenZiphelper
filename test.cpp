#include <iostream>
#include <string>
#include <list>

#include "JsSevenZipHelper/JsSevenZipHelper.h"

int test_compress(int argc, char *argv[])
{
	JsSevenZipHelper::Compress compress;
	std::list<JsSevenZipHelper::Compress::FileListItem> fileList;
	JsSevenZipHelper::FileOutStream *pOutStreamSpec = new JsSevenZipHelper::FileOutStream();
	CMyComPtr<JsSevenZipHelper::FileOutStream> pOutStream = pOutStreamSpec; // ComPtr�� �̿��ϱ� ������ ���� pOutStreamSpec�� delete�� �ʿ� ����.

	pOutStreamSpec->Create("D:\\test.7z", true); // ������ ������ �����Ѵ�.
	
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(true,  false, std::string("D:\\folder"), std::string("folder"))); // ������ �Ǿ� �ϴµ�... �ȵȴ�.. ���� ������ ã�� �ִ� ���̴�.
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(false, false, std::string("D:\\test1.txt"), std::string("test1.txt"))); // �պκ��� ���� ���� ���, �޺κ��� Archive�� ������ ���
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(false, false, std::string("D:\\test2.bin"), std::string("1/test.bin"))); // �̷��� �ڵ����� ���͸��� �����ȴ�.
	fileList.push_back(JsSevenZipHelper::Compress::FileListItem(false, false, std::string("D:\\test2.bin"), std::string("2/test.bin"))); // �̷��� �ڵ����� ���͸��� �����ȴ�.
	
	compress.createArchive(&CLSID_CFormat7z); // 7z ������ Archive�� �����Ѵ�.

	// compress(ISequentialOutStream *pOutStreamSpec, const std::list<FileListItem>& fileList, bool isDefinedPassword, bool isHeaderEncrypt, std::string strPassword)
	// compress(��½�Ʈ��, ���ϸ���Ʈ, ��й�ȣ ������ ������, ������� ��ȣȭ�� ������, ��й�ȣ)
	compress.compress(pOutStream, fileList, true, true, "test1234");

	return 0;
}

int test_extract(int argc, char *argv[])
{
	JsSevenZipHelper::Extract extract;
	std::list<JsSevenZipHelper::Extract::FileListItem> extractFileList;
	JsSevenZipHelper::FileInStream *pInStreamSpec = new JsSevenZipHelper::FileInStream();
	CMyComPtr<JsSevenZipHelper::FileInStream> pInStream = pInStreamSpec; // ComPtr�� �̿��ϱ� ������ ���� pInStreamSpec�� delete�� �ʿ� ����.

	pInStreamSpec->Open("D:\\test.7z"); // ���� ������ ����.

	// openArchive(const GUID *pFormatGUID, IInStream *pInStream, bool isDefinedPassword, std::string strPassword)
	// openArchive(���� ����, �Է½�Ʈ��, ��й�ȣ�� �����Ǿ��ִ���, ��й�ȣ)
	extract.openArchive(&CLSID_CFormat7z, pInStream, true, "test1234");
	
	// extract(const std::string& strExtractRootPath, IArchiveExtractCallback *extractCallback, std::list<FileListItem> *pFileList)
	// extract(���� Ǯ ���͸�, ����� ���� IArchiveExtractCallback�� ����� ���, ���� Ǭ ���� ����Ʈ)
	extract.extract("D:\\extracttest", NULL, &extractFileList);

	return 0;
}

int main(int argc, char *argv[])
{
	test_compress(argc, argv);
	test_extract(argc, argv);
}
