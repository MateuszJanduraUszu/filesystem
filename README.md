# Filesystem

## Build
By default **filesystem.dll/lib** are build for 64-bit Windows 10.  
If you wants to use it in 32-bit system, download **filesystem** directory and open **filesystem.sln** with your Visual Studio.  
Just change target from **x64** to **x86**.  

## Documentation

**\_Expected\_slash**  

inline constexpr char \_Expected\_slash = '\\';  

Expected slash on Windows 10.

**\_Unexpected\_slash**

inline constexpr char \_Unexpected\_slash = '/';

Slash that should be replaced by expected.  
Admittedly it can be used on Windows 10, but it isn't standard slash.

**\_Is\_CharT**

template&lt;class _Ty&gt;  
inline constexpr bool \_Is\_CharT = false;

Checks if the given type is character.

**\_Is\_Src**

template&lt;class _Ty&gt;  
inline constexpr bool \_Is\_Src = false;

Checks if the given type is source (**string**, **u8string**, **u16string**, **u32string** and **wstring**).

**\_Is\_SrcView**

template&lt;class _Ty&gt;  
inline constexpr bool \_Is\_SrcView = false;

Checks if the given type source for view (**string\_view**, **u8string\_view**, **u16stribg\_view**, **u32string\_view** and **wstring\_view**).

**MAX\_PATH**

\#define MAX\_PATH 260

Maximum length of path, on Windows 10.

**error\_type**

enum class error\_type : unsigned int {...};

Error types for \_Throw\_system\_error().

- **invalid\_argument** - throws error from **invalid\_argument** class  
- **length\_error** - throws error from **length\_error** class  
- **runtime\_error** - throws error from **runtime\_error** class

**code\_page**

enum class code\_page {...};

Code pages used in conversion betweens string types.

- **acp** - ansi code page  
- **utf8** - utf8 (default)

**\_Throw\_system\_error**  

\_\_declspec(noreturn) void \_\_cdecl \_Throw\_system\_error(const char* const \_Errpos, const char* const \_Errm, const error\_type _Errc);  

Throws exception from **invalid\_argument**, **length\_error** or **runtime\_error**.

- **\_Errpos** - function where it was called  
- **\_Errm** - error message  
- **\_Errc** - error type from **error\_type**

**\_Convert\_narrow\_to\_wide**  

[[nodiscard]] wstring \_\_cdecl \_Convert\_narrow\_to\_wide(const code\_page \_Code\_page, const string\_view _Input);

Converts char string to wchar_t string.

- **\_Code\_page** - code page from **code\_page** used in conversion  
- **\_Input** - converted string

**\_Convert\_wide\_to\_narrow**

[[nodiscard]] string \_\_cdecl \_Convert\_wide\_to\_narrow(const code\_page \_Code\_page, const wstring\_view \_Input);

Converts wchar_t string to char string.

- **\_Code\_page** - code page from **code\_page** used in conversion  
- **\_Input** - converted string 

**\_Convert\_utf\_to\_narrow**

template&lt;class \_Elem, class \_Traits = char\_traits&lt;\_Elem&gt;&gt;  
[[nodiscard]] string \_\_cdecl \_Convert\_utf\_to\_narrow(const basic\_string\_view&lt;\_Elem, \_Traits&gt; \_Input);

Converts (char8\_t, char16\_t and char32\_t) string to char string.

- **\_Input** - converted string

**\_Convert\_narrow\_to\_utf**

template&lt;class \_Elem, class \_Traits = char\_traits&lt;\_Elem&gt;, class \_Alloc = allocator&lt;\_Elem&gt;&gt;  
[[nodiscard]] basic\_string&lt;\_Elem, \_Traits, \_Alloc&gt; \_\_cdecl \_Convert\_narrow\_to\_utf(const string\_view \_Input);

Converts char string to (char8\_t, char16\_t and char32\_t) string.

- **\_Input** - converted string

**operator+**

[[nodiscard]] path \_\_cdecl operator+(const path& \_Left, const path& \_Right);  
template&lt;class \_CharT&gt;  
[[nodiscard]] path \_\_cdecl operator+(const path& \_Left, const \_CharT* const \_Right);  
template&lt;class \_CharT&gt;  
[[nodiscard]] path \_\_cdecl operator+(const \_CharT* const \_Left, const path& \_Right);  
template&lt;class \_Elem, class \_Traits = char\_traits&lt;\_Elem&gt;, class \_Alloc = allocator&lt;\_Elem&gt;&gt;  
[[nodiscard]] path \_\_cdecl operator+(const path& \_Left, const basic\_string&lt;\_Elem, \_Traits, \_Alloc&gt;& \_Right);  
template&lt;class \_Elem, class \_Traits = char\_traits&lt;\_Elem&gt;, class \_Alloc = allocator&lt;\_Elem&gt;&gt;  
[[nodiscard]] path \_\_cdecl operator+(const basic\_string&lt;\_Elem, \_Traits, \_Alloc&gt;& \_Left, const path& \_Right);

Combines path with any string type.

**operator&gt;&gt;**

template&lt;class \_Elem, class \_Traits = char\_traits&lt;\_Elem&gt;&gt;  
[[nodiscard]] basic\_istream&lt;\_Elem, \_Traits&gt;& \_\_cdecl operator&gt;&gt;(basic\_istream&lt;\_Elem, \_Traits&gt;& \_Stream, path& \_Path);

Assigns content from input stream to **\_Path**.

- **\_Stream** - current input stream  
- **\_Path** - variable that will grab the content from **\_Stream**

**operator&lt;&lt;**

template&lt;class \_Elem, class \_Traits = char\_traits&lt;\_Elem&gt;&gt;  
[[nodiscard]] basic\_ostream&lt;\_Elem, \_Traits&gt;& \_\_cdecl operator&lt;&lt;(basic\_ostream&lt;\_Elem, \_Traits&gt;& \_Stream, const path& \_Path);

Inserts **\_Path** to output stream.

**path::operator=**

path& \_\_cdecl operator=(const path& \_Spource);  
template&lt;class \_CharT&gt;  
path& \_\_cdecl operator=(const \_CharT* const \_Source);  
template&lt;class \_Src&gt;  
path& \_\_cdecl operator=(const \_Src& \_Source);

Assigns **\_Source** to current working path.  
Enables only if **\_CharT** is character type and **\_Src** is source type or source for view type.  
See **\_Is\_CharT**, **\_Is\_Src** and **\_Is\_SrcView**.

- **\_Source** - variable\constant that is assigned to current working path

**path::assign**

path& \_\_cdecl assign(const path& \_Spource);  
template&lt;class \_CharT&gt;  
path& \_\_cdecl assign(const \_CharT* const \_Source);  
template&lt;class \_Src&gt;  
path& \_\_cdecl assign(const \_Src& \_Source);

See **path::operator=**.

**path::operator+=**

path& \_\_cdecl operator+=(const path& \_Added);  
template&lt;class \_CharT&gt;  
path& \_\_cdecl operator+=(const \_CharT* const \_Added);  
template&lt;class \_Src&gt;  
path& \_\_cdecl operator+=(const \_Src& \_Added);

Appends **\_Added** to current working path.

- **\_Added** - variable\constant that is appedned to current working path

**path::append**

path& \_\_cdecl append(const path& \_Added);  
template&lt;class \_CharT&gt;  
path& \_\_cdecl append(const \_CharT* const \_Added);  
template&lt;class \_Src&gt;  
path& \_\_cdecl append(const \_Src& \_Added);

See **path::operator+=**.

**path::operator==**

bool \_\_cdecl operator==(const path& \_Compare);  
template&lt;class \_CharT&gt;  
bool \_\_cdecl operator==(const \_CharT* const \_Compare);  
template&lt;class \_Src&gt;  
bool \_\_cdecl operator==(const \_Src& \_Compare);

Checks if current working path is same as other.  
Enables only if **\_CharT** is character type and **\_Src** is source/source for view type.  
See **\_Is\_CharT**, **\_Is\_Src** and **\_Is\_SrcView**.

- **\_Compare** - variable\constant that is compared with current working path

**path::operator!=**

bool \_\_cdecl operator!=(const path& \_Compare);  
template&lt;class \_CharT&gt;  
bool \_\_cdecl operator!=(const \_CharT* const \_Compare);  
template&lt;class \_Src&gt;  
bool \_\_cdecl operator!=(const \_Src& \_Compare);

Checks if current working path isn't same as other.  
Enables only if **\_CharT** is character type and **\_Src** is source/source for view type.  
See **\_Is\_CharT**, **\_Is\_Src** and **\_Is\_SrcView**.

- **\_Compare** - variable\constant that is compared with current working path