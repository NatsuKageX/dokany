#ifndef DOKANFUSE_H_
#define DOKANFUSE_H_

#include <string>
#include <memory>

#define FUSE_THREAD_COUNT 10
#define DOKAN_DLL L"dokan" DOKAN_MAJOR_API_VERSION L".dll"

// Cygwin64 is LP64 while Windows 64bit is LLP64.
// This why we define macros in the style of inttypes.h for printing DWORDs and ULONGs which are fixed to 32 bit.
#ifdef __LP64__
// DWORD and ULONG are unsigned int
# define PRIxDWORD "%x"
# define PRIuDWORD "%u"
# define PRIxULONG "%x"
# define PRIuULONG "%u"
#else
// DWORD and ULONG are unsigned long int
# define PRIxDWORD "%lx"
# define PRIuDWORD "%lu"
# define PRIxULONG "%lx"
# define PRIuULONG "%lu"
#endif

struct fuse_config
{
  unsigned int umask;
  unsigned int fileumask, dirumask;
  const char *fsname, *volname, *uncname;
  int help;
  int debug;
  int mountManager;
  int readonly;
  int allowIpcBatching;
  int setsignals;
  unsigned int timeoutInSec;
  int removableDrive;
  int networkDrive;
  unsigned long allocationUnitSize;
  unsigned long sectorSize;
  unsigned long max_read;
};

struct fuse_session
{
	fuse_chan *ch;
};

struct dokan_lib_wrap
{
    dokan_lib_wrap();
    ~dokan_lib_wrap();
    dokan_lib_wrap(const dokan_lib_wrap& other) = delete;
	dokan_lib_wrap &operator=(const dokan_lib_wrap &other) = delete;

    bool loaded() const { return dokanDll; }

    typedef ULONG(__stdcall * DokanVersionType)();
	typedef VOID (__stdcall *DokanInitType)();
	typedef VOID(__stdcall *DokanShutdownType)();
	typedef int (__stdcall *DokanMainType)(PVOID,PVOID);
	typedef BOOL (__stdcall *DokanUnmountType)(WCHAR DriveLetter);
	typedef BOOL (__stdcall *DokanRemoveMountPointType)(LPCWSTR MountPoint);
    DokanVersionType ResolvedDokanVersion;
    DokanVersionType ResolvedDokanDriverVersion;
	DokanInitType ResolvedDokanInit = nullptr;
	DokanShutdownType ResolvedDokanShutdown = nullptr;
	DokanMainType ResolvedDokanMain = nullptr;
	DokanUnmountType ResolvedDokanUnmount = nullptr;
	DokanRemoveMountPointType ResolvedDokanRemoveMountPoint = nullptr;

    HMODULE dokanDll = nullptr;
private:
    //This method dynamically loads DOKAN functions
	bool init();
};

struct fuse_chan
{
	fuse_chan() = default;
	~fuse_chan() = default;
	fuse_chan(fuse_chan &other) = delete;
	fuse_chan &operator=(const fuse_chan &other) = delete;

	//This method dynamically loads DOKAN functions
	bool init();

	std::string mountpoint;
    std::shared_ptr<dokan_lib_wrap> dokan_lib;
};

struct fuse
{
	bool within_loop;
	bool mark_exited;
	std::unique_ptr<fuse_chan> ch;
	fuse_session sess;
	fuse_config conf;

	struct fuse_operations ops;
	void *user_data;

	fuse() : within_loop(), mark_exited(false), user_data()
	{
		memset(&conf,0,sizeof(fuse_config));
		memset(&sess, 0, sizeof(fuse_session));
		memset(&ops, 0, sizeof(fuse_operations));
	}
};

#endif //DOKANFUSE_H_
