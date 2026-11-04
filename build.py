"""Build on Windows with MSVC x64; no user-specific installation paths."""
from pathlib import Path
import os, shutil, subprocess, json

ROOT = Path(__file__).resolve().parent

def build():
    env = os.environ.copy()
    vswhere = Path(os.environ.get('ProgramFiles(x86)', 'C:/Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
    if vswhere.exists():
        installs = json.loads(subprocess.check_output([str(vswhere), '-all', '-products', '*', '-format', 'json'], text=True, encoding='utf-8-sig'))
        candidates = [Path(item['installationPath'])/'VC/Auxiliary/Build/vcvars64.bat' for item in installs]
        setup = next((p for p in candidates if p.exists()), None)
        if setup is None:
            raise SystemExit('Install Visual Studio Build Tools with Desktop development with C++.')
        script_dir=ROOT/'build'
        script_dir.mkdir(exist_ok=True)
        env_script=script_dir/'load-msvc.cmd'
        env_script.write_text(f'@echo off\ncall "{setup}" >nul\nif errorlevel 1 exit /b 1\nset\n')
        result = subprocess.run(['cmd.exe', '/d', '/c', str(env_script)], capture_output=True, text=True, check=True)
        for line in result.stdout.splitlines():
            if '=' in line and not line.startswith('='):
                key, value = line.split('=', 1)
                env[key] = value
    elif not shutil.which('cl'):
        raise SystemExit('Run in an x64 Native Tools Command Prompt, or install Visual Studio C++ Build Tools.')
    compiler = shutil.which('cl', path=env.get('Path', env.get('PATH')))
    if not compiler:
        raise SystemExit('MSVC cl.exe was not found after environment setup.')
    target = ROOT / 'build'
    target.mkdir(exist_ok=True)
    flags = ['/nologo', '/O2', '/std:c++17', '/EHsc', '/DNDEBUG', '/W4']
    subprocess.run([compiler, *flags, str(ROOT/'benchmarks/matched.cpp'), str(ROOT/'src/binary-heap/dijkstra_binary.cpp'), '/Fe:matched.exe'], cwd=target, env=env, check=True)
    subprocess.run([compiler, *flags, '/I'+str(ROOT/'src/binary-heap'), str(ROOT/'tests/binary_heap_test.cpp'), '/Fe:james-heap-test.exe'], cwd=target, env=env, check=True)
    print('Built build/matched.exe and build/james-heap-test.exe (MSVC x64).')

if __name__ == '__main__':
    build()
