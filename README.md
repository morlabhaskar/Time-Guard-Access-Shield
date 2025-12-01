# 🛡️ TimeGuard Access Shield — Password-Controlled Time-Restricted Machine Operation System

TimeGuard Access Shield is an embedded access-control system built using the **LPC2148** microcontroller.  
It combines **secure password authentication**, **RTC-based scheduling**, and an **interrupt-driven configuration menu** to ensure that only authorized users can operate a device — and only at allowed times.

---
## 📸 Project Images

### Proteus Image
![Block Diagram](images/slider.gif)

---

## 🔐 Features

- 🕒 **Real-Time Clock Display** (Time, Date, Day)
- 🔑 **Password-Based Authentication** (Changeable Password)
- 👁️ **Password Show / Hide Feature using Eye Toggle**
- 🚪 **Entry Access Only in Working Hours**
- ⚙️ **Admin Menu via EINT0 Interrupt**
  - Change Date
  - Change Time
  - Change Password
  - Change Working Hours
- ⚡ **Edge-Triggered EINT0 Interrupt Handling**
- 🎨 LCD Custom Characters (Lock, Eye, Success, Warning)
- ❌ Blocks login after 3 failed attempts
- 📟 Full RTC configuration and live display

---

## 🧩 Hardware Requirements

- LPC2129 / LPC2148 (or any LPC21xx)
- 16x2 LCD (HD44780)
- 4x4 Matrix Keypad
- EINT0 External Interrupt Switch
- RTC Module (DS1307 or compatible)
- Power Supply 5V
- Wires, Breadboard / PCB

---

## 📁 Project Structure

```
/project
│-- main.c
│-- lcd.c / lcd.h
│-- rtc.c / rtc.h
│-- kpm.c / kpm.h
│-- system_init.c
│-- delay.c / delay.h
│-- arduino.c
│-- pin_connect_block.h
│-- types.h
```

---

## ⚙️ Key Functionalities

### 1️⃣ Login Authentication

- Accepts 4-digit password
- Shows `*` by default
- Eye button toggles **Show / Hide**
- Supports backspace with `C`
- Auto-block after 3 wrong attempts

---

### 2️⃣ Password Show / Hide Feature

A **custom CGRAM eye icon** is displayed at the right side.  
Pressing the `E` key toggles:

- 👁️ **Show Mode** → Displays actual digits  
- 🔒 **Hide Mode** → Displays `*` for all digits (including previously entered ones)

This ensures full privacy during password entry.

---

### 3️⃣ Working Hours Access Control

System allows login **only between Entry & Exit hours**:

```c
if ((EnHr <= cHour) && (ExHr >= cHour)) return 1;
```

---

### 4️⃣ Admin Menu (via EINT0 Interrupt)

When EINT0 switch is pressed, system shows:

```
1 – Change Date
2 – Change Time
3 – Change Password
4 – Employee Working Hours
5 – Exit
```

---

### 5️⃣ RTC Live Display

Continuously updates:

- Time → HH:MM:SS  
- Date → DD/MM/YYYY  
- Day → SUN to SAT  

---

## 🚀 How to Use

### ▶️ Startup

- Shows title screen  
- Shows RTC continuously  
- Press ENTRY switch → Password Login Screen  

### ▶️ Admin Mode

- Press EINT0 button  
- Press **1** to open admin menu  

---

## 🛠 Build & Flash

Use **Keil uVision**, **Arm-GCC**, FlashMagic.

---

## 📸 LCD Icons (CGRAM)

- Lock icon  
- Eye icon (show/hide)  
- Success tick  
- Warning sign  
- Clock icon  

---

## 🏁 Conclusion
TimeGuard Access Shield ensures **secure, reliable, and time-restricted** machine operation — ideal for labs, workshops, and industrial environments.
