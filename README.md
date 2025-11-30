# Hypercall – Windows Kernel Driver for Secure Telemetry & System Introspection

**Hypercall** is a Windows kernel driver and user-mode client showcasing trusted communication and low-level telemetry collection inside the Windows operating system.  
This repository demonstrates core skills required for modern **anti-cheat engineering**: kernel development, memory and code inspection, secure driver communication, and controlled trust boundaries suitable for behavioral telemetry and integrity enforcement.

> **Note:** This repository is a **demo version** of a substantially larger kernel project.  
The original implementation included a complete operational framework that cannot be published due to contractual and legal constraints.  
This release preserves only the safe architectural core — the communication layer and telemetry model — allowing the engineering concepts to be reviewed without exposing proprietary or harmful functionality.

---

## 🎯 Project Objectives

Competitive online environments must detect and prevent increasingly sophisticated cheats, such as:

- Kernel-resident code without legitimate loaders  
- Manual-mapped modules bypassing Windows' security model  
- Tampering of protected memory pages  
- Runtime control-flow manipulation and stealth hooking  

**Hypercall** explores the mechanisms needed to counter these techniques by implementing a **trusted, minimal kernel component** capable of:

- Receiving structured user-mode requests  
- Inspecting internal system state safely  
- Returning validated telemetry that cannot be spoofed from user-mode  

These concepts directly map to the architectural foundations of **modern anti-cheat clients**, including FACEIT’s Community Integrity systems.

---

## 🧱 Architecture Overview

```
UserCall.exe  (User Mode)
     │ Issues IOCTL requests
     ▼
────────── Trust Boundary ──────────
     ▲
hypercall.sys  (Kernel Driver)
     │ Collects and validates telemetry
     ▼ Returns structured data
```

The driver exposes an IOCTL surface that is intentionally small, auditable, and safe — a key characteristic of production-ready anti-cheat drivers.

---

## 📁 Repository Structure

```
hypercall/
├─ src/              # Kernel driver source (C/C++)
├─ UserCall/         # User-mode client for IOCTL interaction
├─ hypercall.inf     # Driver installation file
├─ hypercall.sln     # Visual Studio solution
└─ README.md
```

### Kernel Driver

Implements:

- `DriverEntry` and orderly unload paths  
- IRP dispatch and IOCTL handlers  
- Parameter validation to enforce trust boundaries  
- Kernel-side helper routines for telemetry retrieval  

### User-Mode Client (`UserCall`)

- Opens a handle to the device object  
- Sends parameterized telemetry requests  
- Prints validated kernel responses  

This separation models real-world anti-cheat communication flows.

---

## 🔍 Technical Highlights

| Feature | Relevance |
|--------|-----------|
| Windows kernel driver development | Required for privileged anti-cheat agents |
| IOCTL-based IPC | Clean user/kernel separation with strict validation |
| Driver lifecycle management | Stability in persistent client environments |
| Kernel telemetry hooks | Foundation for cheat detection heuristics |
| Minimal, auditable attack surface | Reduces cheat evasion opportunities |
| Extensible control paths | Supports progressive deployment of new checks |

---

## 🔧 Build & Installation

### Requirements

- Windows 10/11 x64
- Visual Studio + C/C++ toolchain
- Windows Driver Kit (WDK)
- Admin privileges (for driver load/install)

### Steps

```bash
git clone https://github.com/Aspasia1337/hypercall.git
cd hypercall
```

1. Open `hypercall.sln` in Visual Studio  
2. Select configuration `x64` / `Release` or `Debug`  
3. Build the driver and user-mode client  
4. Install the driver via `hypercall.inf` or `pnputil`  
5. Start the driver service  
6. Run `UserCall.exe` to query kernel telemetry  

---

## 🚀 Extension Roadmap

Hypercall is intentionally focused and modular. Potential extensions include:

- Code section hashing for `.text` region integrity  
- Detection of manually mapped images and unusual memory layouts  
- Monitoring of suspicious thread start addresses  
- Hypervisor-based triggering for out-of-band integrity checks  
- Backend telemetry submission for behavioral correlation at scale  

These features align directly with FACEIT’s real-world anti-cheat requirements.

---

## 🛠️ Production & Usage Context

Hypercall is a **public, stripped‑down demonstration** of a larger, production‑grade **kernel cheat framework** originally developed for a private client.  
The private version included:

- Custom kernel driver injection and stealth execution flows
- Kernel-mode memory and module manipulation routines
- Process introspection and thread origin tracking
- Anti-detection countermeasures against commercial anti-cheat products
- Privilege boundary abuse and telemetry redirection mechanisms

This public release **does not** include any exploit logic, anti-cheat bypasses, loaders, or functionality that could be used to create a working cheat.  
Only the **communication architecture** and **trusted telemetry backbone** have been preserved to demonstrate the engineering principles behind the original project.

The knowledge gained from offensive kernel development directly informs **defensive anti-cheat engineering**—understanding how adversaries execute code, conceal state, and tamper with telemetry is essential to detecting and preventing such behavior at scale.

---

## ⚖️ Disclaimer

This project is provided **solely for educational and research purposes**.  
It is **not** intended for cheating, bypassing protections, or violating any Terms of Service.

Misuse is strictly discouraged and entirely the responsibility of the user.

---

## 👤 Author


> *“Fair competition starts with trusted execution. Hypercall is a foundation toward that goal.”*