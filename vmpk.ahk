; This AutoHotkey script captures almost all keystrokes
; when CapsLock is enabled, and sends them to the vmpk.exe application

#If CapsLocked()
^j::
^i::
a::
b::
c::
d::
e::
f::
g::
h::
i::
j::
k::
l::
m::
n::
o::
p::
q::
r::
s::
t::
u::
v::
w::
x::
y::
z::
Control::
Alt::
Shift::
F1::
F2::
F3::
F4::
F5::
F6::
F7::
F8::
F9::
F10::
F11::
F12::
1::
2::
3::
4::
5::
6::
7::
8::
9::
0::
Delete::
Insert::
Home::
End::
PgUp::
PgDn::
BS::
Enter::
Tab::
Space::
Numpad0::
Numpad1::
Numpad2::
Numpad3::
Numpad4::
Numpad5::
Numpad6::
Numpad7::
Numpad8::
Numpad9::
NumpadDiv::
NumpadMult::
NumpadAdd::
NumpadSub::
NumpadEnter::
NumpadDot::
`::
[::
]::
\::
,::
.::
/::
`;::
'::
	Suspend On
	
	SetKeyDelay, 0, 40
	
	;Send, My First Script
	SetTitleMatchMode, 2
	;if WinExist("ahk_exe notepad++.exe")
	ControlSend, ahk_parent, {%A_ThisHotkey%},  ahk_exe vmpk.exe
	;MsgBox %A_ThisHotkey%
	
	Suspend Off
	return

CapsLocked() {
	state := GetKeyState("Capslock", "T")  ; 1 if CapsLock is ON, 0 otherwise.
	return (state=1)
}
