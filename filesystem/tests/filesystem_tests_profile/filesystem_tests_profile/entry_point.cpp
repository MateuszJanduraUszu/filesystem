// entry_point.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem.hpp>
#ifdef _M_X64
#ifdef _DEBUG
#pragma comment(lib, R"(x64\Debug\filesystem.lib)")
#else // ^^^ _DEBUG ^^^ / vvv NDEBUG vvv
#pragma comment(lib, R"(x64\Release\filesystem.lib)")
#endif // _DEBUG
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
#ifdef _DEBUG
#pragma comment(lib, R"(Debug\filesystem.lib)")
#else // ^^^ _DEBUG ^^^ / vvv NDEBUG vvv
#pragma comment(lib, R"(Release\filesystem.lib)")
#endif // _DEBUG
#endif // _M_IX86

using namespace filesystem;

#define _CLEAR_CONSOLE  system("cls")
#define _WAIT           system("pause")
#define _INVALID_CHOICE _STD cout << "No such choice.\n"; _STD cin.get(); _CLEAR_CONSOLE

// STRUCT _Profile_data
struct _Profile_data { // basic informations about the user
    string _Username{};
    string _Login{};
    string _Password{};
    bool _Logoff;
    uint32_t _Age{};
    string _Phone_number{};
    
    struct {
        uint32_t _Day;
        uint32_t _Month;
        uint32_t _Year;
    } _Birthday_date{};
};

// CLASS _Load_profile
class _Load_profile { // read the file with profile informations
public:
    _Load_profile() noexcept : _Mydata(_Profile_data{}), _Mypath(path{}) {}
    
    explicit _Load_profile(const _Load_profile& _Other) noexcept
        : _Mydata(_Other._Mydata), _Mypath(_Other._Mypath) {}

    explicit _Load_profile(_Load_profile&& _Other) noexcept
        : _Mydata(_Other._Mydata), _Mypath(_Other._Mypath) {}

    virtual ~_Load_profile() noexcept {}

    explicit _Load_profile(const path& _Path) : _Mydata(_Profile_data{}), _Mypath(_Path) {
        _Load();
    }

    _NODISCARD path& _Getpath() noexcept { // for modify
        return _Mypath;
    }

    _NODISCARD path _Getpath() const noexcept { // for view
        return _Mypath;
    }

    _NODISCARD _Profile_data _Getdata() const noexcept { // only for view
        return _Mydata;
    }

    void _Load() { // get profile settings
        if (!exists(_Mypath)) {
            _Throw_fs_error("file with settings not found", error_type::runtime_error, "_Load");
        }

        auto _All{read_all(_Mypath)};
        for (auto& _Elem : _All) {
            _Elem.erase(_Elem.begin(), _Elem.begin() + _Elem.find('=') + 2); // leave only value
            _Elem.pop_back();
        }

        _Mydata._Username             = _All[0];
        _Mydata._Login                = _All[1];
        _Mydata._Password             = _All[2];
        _Mydata._Logoff               = _All[3] == "true";
        _Mydata._Age                  = _STD stoi(_All[4]);
        _Mydata._Phone_number         = _All[5];
        _Mydata._Birthday_date._Day   = _STD stoi(_All[6]);
        _Mydata._Birthday_date._Month = _STD stoi(_All[7]);
        _Mydata._Birthday_date._Year  = _STD stoi(_All[8]);
    }

private:
    _Profile_data _Mydata; // profile basic informations
    path _Mypath; // path to the file with profile settings
};

// CLASS _Save_profile
class _Save_profile { // overwrites the file with profile informations
public:
    _Save_profile() noexcept : _Mydata(_Profile_data{}), _Mypath(path{}) {}

    explicit _Save_profile(const _Save_profile& _Other) noexcept
        : _Mydata(_Other._Mydata), _Mypath(_Other._Mypath) {}

    explicit _Save_profile(_Save_profile&& _Other) noexcept
        : _Mydata(_Other._Mydata), _Mypath(_Other._Mypath) {}

    virtual ~_Save_profile() noexcept {}

    explicit _Save_profile(const path& _Path) : _Mydata(_Profile_data{}), _Mypath(_Path) {}
    
    explicit _Save_profile(const path& _Path, _Profile_data* const _Data) : _Mydata(*_Data), _Mypath(_Path) {}

    _NODISCARD path& _Getpath() noexcept { // for modify
        return _Mypath;
    }

    _NODISCARD path _Getpath() const noexcept { // for view
        return _Mypath;
    }

    _NODISCARD _Profile_data* _Getdata() noexcept { // for modify
        return &_Mydata;
    }

    _NODISCARD _Profile_data _Getdata() const noexcept { // for view
        return _Mydata;
    }

    void _Save() { // change profile settings
        if (!exists(_Mypath)) {
            _Throw_fs_error("file with settings not found", error_type::runtime_error, "_Load");
        }

        const string& _Logoff            = (_Mydata._Logoff ? "true" : "false");
        const array<string, 9> _Writable = {
            R"(username=")" + _Mydata._Username + R"(")",
            R"(login=")" + _Mydata._Login + R"(")",
            R"(password=")" + _Mydata._Password + R"(")",
            R"(logoff=")" + _Logoff + R"(")",
            R"(age=")" + _STD to_string(_Mydata._Age) + R"(")",
            R"(phone_number=")" + _Mydata._Phone_number + R"(")",
            R"(bth_day=")" + _STD to_string(_Mydata._Birthday_date._Day) + R"(")",
            R"(bth_month=")" + _STD to_string(_Mydata._Birthday_date._Month) + R"(")",
            R"(bth_year=")" + _STD to_string(_Mydata._Birthday_date._Year) + R"(")"
        };

        (void) clear(_Mypath); // we only use settings for one profile, so the file is entirely our

        for (const auto& _Elem : _Writable) {
            (void) write_back(_Mypath, _Elem.c_str());
        }
    }
    
private:
    _Profile_data _Mydata; // profile informations ready for save
    path _Mypath; // path to the file with profile informations
};

// CLASS _Application
class _Application {
public:
    _Application()                             = delete;
    explicit _Application(const _Application&) = delete;
    explicit _Application(_Application&&)      = delete;
    virtual ~_Application()                    = default;

    explicit _Application(const path& _Path) : _Mypath(_Path) {
        _Init();
    }

    void _Start() noexcept {
        _Update();
    }
    
private:
    void _Pre_menu(int _Choice = int()) noexcept {
        _CLEAR_CONSOLE;
        _STD cout << "Welcome in application\n";
        _STD cout << "1. Login\n";
        _STD cout << "2. Quit\n";
        _STD cout << "Your choice: ";
        _STD cin >> _Choice;
        _CLEAR_CONSOLE;

        switch (_Choice) {
        case 1:
            _Mypos = _Pos::_Login;
            break;
        case 2:
            _CSTD exit(EXIT_SUCCESS);
            break;
        default: // nothing has changed
            _INVALID_CHOICE;
            break;
        }

        _Update();
    }

    void _Login() noexcept {
        _CLEAR_CONSOLE;
        if (_Mystate == _Verification_state::_Verified) {
            _Mypos = _Pos::_Menu;
            _Update();
        } else {
            _CLEAR_CONSOLE;
            string _VLogin;
            string _VPassword;
            _STD cout << "Enter login: ";
            _STD cin >> _VLogin;
            _STD cout << "Enter password: ";
            _STD cin >> _VPassword;
            _CLEAR_CONSOLE;
            
            if (int _Choice; _VLogin == _Mydata._Login && _VPassword == _Mydata._Password) {
                _Mystate = _Verification_state::_Verified;
                _Mypos   = _Pos::_Menu;
                _Update();
            } else { // invalid login data
                do {
                    _CLEAR_CONSOLE;
                    _STD cout << "Wrong login or password.\n";
                    _STD cout << "1. Try again\n";
                    _STD cout << "2. Back to the menu\n";
                    _STD cout << "Your choice: ";
                    _STD cin >> _Choice;
                    _CLEAR_CONSOLE;

                    if (_Choice == 1) { // nothing has changed; try again
                        break;
                    } else if (_Choice == 2) { // back to the application main menu
                        _Mypos = _Pos::_Pre_menu;
                        break;
                    } else {
                        continue;
                    }
                } while (true);

                _Update();
            }
        }
    }

    void _Menu(int _Choice = int()) {
        _CLEAR_CONSOLE;
        _STD cout << "Welcome " << _Mydata._Username << ".\n";
        _STD cout << "1. See settings\n";
        _STD cout << "2. Change settings\n";
        _STD cout << "3. Logout\n";
        _STD cout << "Your choice: ";
        _STD cin >> _Choice;
        _CLEAR_CONSOLE;

        switch (_Choice) {
        case 1:
            _Mypos = _Pos::_Profile;
            break;
        case 2:
            _Mypos = _Pos::_Change_profile;
            break;
        case 3:
            _Mypos = _Pos::_Pre_menu;

            if (_Mydata._Logoff) { // must login again
                _Mystate = _Verification_state::_No_verified;
            }

            break;
        default: // nothing has changed
            _INVALID_CHOICE;
            break;
        }

        _Update();
    }

    void _Profile() noexcept {
        _CLEAR_CONSOLE;
        _STD cout << "Username: " << _Mydata._Username << "\n";
        _STD cout << "Login: " << _Mydata._Login << "\n";
        _STD cout << "Password: " << _Mydata._Password << "\n";
        _STD cout << "Autimatic logout: " << (_Mydata._Logoff ? "yes" : "no") << "\n";
        _STD cout << "Age: " << _Mydata._Age << "\n";
        _STD cout << "Phone number: " << _Mydata._Phone_number << "\n";
        _STD cout << "Date of birth: " << _Make_date() + "\n";
        _WAIT;
        _CLEAR_CONSOLE;

        _Mypos = _Pos::_Menu; // back to the profile menu
        _Update();
    }

    void _Change_profile(int _Choice = int(), string _Newdata = string()) {
        _CLEAR_CONSOLE;
        _STD cout << "Change settings.\n";
        _STD cout << "1. Change username\n";
        _STD cout << "2. Change login\n";
        _STD cout << "3. Change password\n";
        _STD cout << "4. Change automatic logout\n";
        _STD cout << "5. Change age\n";
        _STD cout << "6. Change phone number\n";
        _STD cout << "7. Change date of birth\n";
        _STD cout << "8. Back to the profile menu\n";
        _STD cout << "Your choice: ";
        _STD cin >> _Choice;
        _CLEAR_CONSOLE;

        switch (_Choice) {
        case 1: // change username
            _STD cout << "New username: ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (_Newdata != _Mydata._Username) { // do nothing if no changes
                _Mydata._Username             = _Newdata;
                _Mysave._Getdata()->_Username = _Newdata;
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 2: // change login
            _STD cout << "New login: ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (_Newdata != _Mydata._Login) { // do nothing if no changes
                _Mydata._Login             = _Newdata;
                _Mysave._Getdata()->_Login = _Newdata;
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 3: // change password
            _STD cout << "New password: ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (_Newdata != _Mydata._Password) {
                _Mydata._Password             = _Newdata;
                _Mysave._Getdata()->_Password = _Newdata;
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 4: // change automatic logout
            _STD cout << "New automatic logout: ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (_Newdata != (_Mydata._Logoff ? "yes" : "no")) {
                _Mydata._Logoff             = _Newdata == "yes";
                _Mysave._Getdata()->_Logoff = _Newdata == "yes";
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 5: // change age
            _STD cout << "New age: ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (const uint32_t _SAge = static_cast<uint32_t>(_STD stoi(_Newdata)); _SAge != _Mydata._Age) {
                _Mydata._Age             = _SAge;
                _Mysave._Getdata()->_Age = _SAge;
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 6: // change phone number
            _STD cout << "New phone number: ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (_Newdata != _Mydata._Phone_number) {
                _Mydata._Phone_number             = _Newdata;
                _Mysave._Getdata()->_Phone_number = _Newdata;
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 7: // change date of birth
            _STD cout << "New date of birth (xx.xx.xxxx): ";
            _STD cin >> _Newdata;
            _CLEAR_CONSOLE;

            if (_Newdata != _Make_date()) {
                _Mydata._Birthday_date._Day               = _STD stoi(string(_Newdata.begin(), _Newdata.begin() + 2));
                _Mydata._Birthday_date._Month             = _STD stoi(string(_Newdata.begin() + 3, _Newdata.begin() + 5));
                _Mydata._Birthday_date._Year              = _STD stoi(string(_Newdata.begin() + 6, _Newdata.end()));
                _Mysave._Getdata()->_Birthday_date._Day   = _Mydata._Birthday_date._Day;
                _Mysave._Getdata()->_Birthday_date._Month = _Mydata._Birthday_date._Month;
                _Mysave._Getdata()->_Birthday_date._Year  = _Mydata._Birthday_date._Year;
                _Mysave._Save();
            }

            _Mypos = _Pos::_Change_profile;
            break;
        case 8: // back to the profile menu
            _Mypos = _Pos::_Menu;
            break;
        default:
            _INVALID_CHOICE;
            break;
        }

        _Update();
    }

    void _Update() noexcept {
        _CLEAR_CONSOLE;
        switch (_Mypos) {
        case _Pos::_Pre_menu:
            _Pre_menu();
            break;
        case _Pos::_Login:
            _Login();
            break;
        case _Pos::_Menu:
            _Menu();
            break;
        case _Pos::_Profile:
            _Profile();
            break;
        case _Pos::_Change_profile:
            _Change_profile();
            break;
        default: // invalid
            break;
        }
    }

    void _Init() {
        _Myload._Getpath() = _Mypath; // update path for loading
        _Mysave._Getpath() = _Mypath; // update path for saving
        _Myload._Load(); // update settings
        _Mydata             = _Myload._Getdata();
        *_Mysave._Getdata() = _Mydata; // update data for saving
        _Mystate            = _Mydata._Logoff ? _Verification_state::_No_verified : _Verification_state::_Verified;
        _Mypos              = _Pos::_Pre_menu;
    }

    _NODISCARD string _Make_date() noexcept {
        const string& _SDay   = (_Mydata._Birthday_date._Day < 10 ? "0" : "") + _STD to_string(_Mydata._Birthday_date._Day);
        const string& _SMonth = (_Mydata._Birthday_date._Month < 10 ? "0" : "") + _STD to_string(_Mydata._Birthday_date._Month);
        const string& _SYear  = _STD to_string(_Mydata._Birthday_date._Year);
        return _SDay + "." + _SMonth + "." + _SYear;
    }

    enum class _Pos { // current position in application
        _Pre_menu, // before logging
        _Login,
        _Menu,
        _Profile,
        _Change_profile
    };

    enum class _Verification_state { // login state
        _Verified,
        _No_verified
    };

    _Pos _Mypos; // current position in application
    _Verification_state _Mystate; // checks if user must login to access data
    _Profile_data _Mydata; // informations about the profile
    _Load_profile _Myload;
    _Save_profile _Mysave;
    path _Mypath;
};

// To avoid warning C4001: main() must be __cdecl
#ifndef _CDECL_OR_STDCALL
#ifdef _M_IX86
#define _CDECL_OR_STDCALL __cdecl
#else // ^^^ _M_IX86 ^^^ / vvv _M_X64 vvv
#define _CDECL_OR_STDCALL __stdcall
#endif // _M_IX86
#endif // _CDECL_OR_STDCALL

int _CDECL_OR_STDCALL main(int _Count, char** _Params) {
    const path& _Path{__FILE__};
    (void) const_cast<path&>(_Path).replace_file("data.txt");

    _Application _App(_Path);
    _App._Start();
    return EXIT_SUCCESS;
}

#undef _CDECL_OR_STDCALL
#undef _INVALID_CHOICE
#undef _WAIT
#undef _CLEAR_CONSOLE