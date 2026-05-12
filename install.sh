#!/usr/bin/env bash
set -euo pipefail

missing_tools=()
make_args=()
os_name="$(uname -s)"

has_boost_headers()
{
    [ -d "$1/boost" ]
}

find_boost_include()
{
    if [ -n "${BOOST_DIR:-}" ] && has_boost_headers "$BOOST_DIR"; then
        printf '%s\n' "$BOOST_DIR"
        return 0
    fi

    if has_boost_headers "/usr/include"; then
        printf '%s\n' "/usr/include"
        return 0
    fi

    if has_boost_headers "/usr/local/include"; then
        printf '%s\n' "/usr/local/include"
        return 0
    fi

    if has_boost_headers "/opt/homebrew/include"; then
        printf '%s\n' "/opt/homebrew/include"
        return 0
    fi

    if command -v brew >/dev/null 2>&1; then
        boost_prefix="$(brew --prefix boost 2>/dev/null || true)"
        if [ -n "$boost_prefix" ] && has_boost_headers "$boost_prefix/include"; then
            printf '%s\n' "$boost_prefix/include"
            return 0
        fi
    fi

    return 1
}

install_linux_dependencies()
{
    if ! command -v apt-get >/dev/null 2>&1; then
        return 1
    fi

    echo "Installing required packages with apt-get..."
    sudo apt-get update
    sudo apt-get install -y build-essential libboost-all-dev
}

install_macos_boost()
{
    if ! command -v brew >/dev/null 2>&1; then
        echo "Homebrew was not found. Install Homebrew first, then run ./install.sh again."
        return 1
    fi

    echo "Installing Boost with Homebrew..."
    brew install boost
}

check_required_tools()
{
    missing_tools=()

    for tool in make ar; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done

    if [ "$os_name" = "Linux" ] && ! command -v gcc-ar >/dev/null 2>&1; then
        missing_tools+=("gcc-ar")
    fi

    if ! command -v gcc >/dev/null 2>&1 && ! command -v clang >/dev/null 2>&1; then
        missing_tools+=("gcc or clang")
    fi

    if ! command -v g++ >/dev/null 2>&1 && ! command -v clang++ >/dev/null 2>&1; then
        missing_tools+=("g++ or clang++")
    fi
}

boost_include="$(find_boost_include || true)"

if [ -z "$boost_include" ]; then
    echo "Boost was not found. Trying to install it automatically..."

    case "$os_name" in
        Linux)
            install_linux_dependencies || {
                echo "Automatic Boost installation failed. Install Boost with your system package manager or set BOOST_DIR."
                exit 1
            }
            ;;
        Darwin)
            install_macos_boost || {
                echo "Automatic Boost installation failed. Install Boost with Homebrew or set BOOST_DIR."
                exit 1
            }
            ;;
        *)
            echo "Automatic Boost installation is supported only on Linux with apt-get and on macOS with Homebrew."
            echo "Set BOOST_DIR to the directory containing the boost/ headers."
            exit 1
            ;;
    esac

    boost_include="$(find_boost_include || true)"

    if [ -z "$boost_include" ]; then
        echo "Boost is still not available after installation."
        echo "Set BOOST_DIR to the directory containing the boost/ headers."
        exit 1
    fi
fi

check_required_tools

if [ "${#missing_tools[@]}" -ne 0 ] && [ "$os_name" = "Linux" ] && command -v apt-get >/dev/null 2>&1; then
    echo "Some build tools are missing. Trying to install them automatically..."
    install_linux_dependencies || true
    check_required_tools
fi

if [ "${#missing_tools[@]}" -ne 0 ]; then
    echo "Missing required build tools:"
    printf ' - %s\n' "${missing_tools[@]}"
    exit 1
fi

make_args+=("BOOST_DIR=$boost_include")

if [ "$os_name" = "Darwin" ]; then
    make_args+=("CC=clang" "CXX=clang++")
fi

echo "Building PlanG..."
make "${make_args[@]}"

echo
echo "Build finished successfully"
echo "Run the program with: ./plang"