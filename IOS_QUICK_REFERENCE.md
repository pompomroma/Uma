# iOS Build - Quick Reference Card

## ⚡ Quick Commands

### On macOS
```bash
cd /path/to/RacingGame3D
./setup_ios.sh
open ios/build/RacingGame3DiOS.xcodeproj
# Then: Connect iPhone, Click ▶️ in Xcode
```

### On Codespaces/Linux
```
❌ Cannot build iOS apps here
✅ Download project to Mac, then follow "On macOS" steps
```

---

## 🆘 Quick Troubleshooting

| Error | Solution |
|-------|----------|
| "must be run from project root" | `cd /path/to/RacingGame3D` |
| "not on macOS" | Transfer to Mac |
| "Xcode not installed" | Install from App Store |
| "Failed to code sign" | Xcode → Preferences → Accounts → Add Apple ID |
| "Untrusted Developer" | iPhone Settings → General → VPN & Device Management → Trust |
| "No such file: build_ios.sh" | `cd /path/to/RacingGame3D` first |

---

## 📚 Documentation Hierarchy

1. **INSTALL_IOS.md** ← 🌟 START HERE (Complete guide)
2. **QUICKSTART_IOS.md** (5-step guide)
3. **IOS_BUILD_GUIDE.md** (Technical details)
4. **This file** (Quick reference)

---

## ✅ Prerequisites Checklist

- [ ] Mac with macOS 10.15+
- [ ] Xcode installed
- [ ] iPhone with iOS 13.0+
- [ ] USB cable
- [ ] Apple ID (free)

---

## 🎯 One-Line Summary

**Codespaces/Linux**: Download to Mac first  
**Mac**: `./setup_ios.sh` → Open Xcode → Click Play

---

**Need help?** → Read `INSTALL_IOS.md`
