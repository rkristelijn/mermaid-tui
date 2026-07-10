#!/usr/bin/env bash
# setup.sh — Install all development dependencies at pinned versions.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

cd "$(dirname "$0")/../.."

# shellcheck source=../../.config/versions.env
source .config/versions.env

has() { command -v "$1" >/dev/null 2>&1; }

detect_platform() {
  IS_MAC=false
  IS_LINUX=false

  if [[ "$(uname -s)" == "Darwin" ]]; then
    IS_MAC=true
    if ! has brew; then
      echo "ERROR: macOS detected but Homebrew not found. Install from https://brew.sh" >&2
      exit 1
    fi
    PKG="brew install"
  elif has apt-get; then
    IS_LINUX=true
    PKG="sudo apt-get install -y"
  else
    echo "ERROR: no supported package manager found (brew or apt)" >&2
    exit 1
  fi
}

install_pkg() {
  local name="$1" brew_pkg="$2" apt_pkg="$3"
  if [[ "${IS_MAC}" == true ]]; then
    echo "  Installing ${name} (brew install ${brew_pkg})..."
    brew install "${brew_pkg}"
  elif [[ "${apt_pkg}" == "SKIP" ]]; then
    echo "  SKIP: ${name} not available via apt"
    return 1
  else
    echo "  Installing ${name} (apt-get install ${apt_pkg})..."
    sudo apt-get install -y "${apt_pkg}"
  fi
}

install_llvm_tools() {
  local ver="${LLVM_VERSION}"
  if [[ "${IS_MAC}" == true ]]; then
    if ! has clang-format; then
      echo "  Installing LLVM ${ver} (brew install llvm)..."
      brew install llvm
      echo "  NOTE: add /opt/homebrew/opt/llvm/bin to your PATH"
    fi
  else
    local need_install=false
    has "clang-format-${ver}" || need_install=true
    has "clang-tidy-${ver}" || need_install=true
    if [[ "${need_install}" == true ]]; then
      echo "  Installing LLVM ${ver} tools from apt.llvm.org..."
      wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key |
        sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc >/dev/null
      # shellcheck source=/dev/null
      source /etc/os-release
      local codename="${UBUNTU_CODENAME:-${VERSION_CODENAME}}"
      echo "deb http://apt.llvm.org/${codename}/ llvm-toolchain-${codename} main" |
        sudo tee /etc/apt/sources.list.d/llvm.list >/dev/null
      sudo apt-get update -qq
      sudo apt-get install -y "clang-format-${ver}" "clang-tidy-${ver}"
      has clang-format || sudo ln -sf "/usr/bin/clang-format-${ver}" /usr/bin/clang-format
      has clang-tidy || sudo ln -sf "/usr/bin/clang-tidy-${ver}" /usr/bin/clang-tidy
    fi
  fi
}

install_doxygen() {
  local ver="${DOXYGEN_VERSION}"
  local current
  current="$(doxygen --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo "0")"
  if [[ "${current}" != "0" ]]; then
    local min
    min="$(printf '%s\n%s' "${ver}" "${current}" | sort -V | head -n1)"
    [[ "${min}" == "${ver}" ]] && return 0
  fi
  if [[ "${IS_MAC}" == true ]]; then
    echo "  Installing doxygen (brew install doxygen)..."
    brew install doxygen
  else
    echo "  Installing doxygen ${ver} (direct download)..."
    wget -q "https://doxygen.nl/files/doxygen-${ver}.linux.bin.tar.gz" -O "/tmp/doxygen-${ver}.tar.gz"
    tar -xzf "/tmp/doxygen-${ver}.tar.gz" -C /tmp
    sudo cp "/tmp/doxygen-${ver}/bin/doxygen" /usr/local/bin/doxygen
    rm -rf "/tmp/doxygen-${ver}" "/tmp/doxygen-${ver}.tar.gz"
  fi
}

install_semgrep() {
  has semgrep && return 0
  if [[ "${IS_MAC}" == true ]]; then
    echo "  Installing semgrep (brew install semgrep)..."
    brew install semgrep
  else
    has pipx || sudo apt-get install -y pipx
    echo "  Installing semgrep (pipx install semgrep)..."
    pipx install semgrep
  fi
}

install_rumdl() {
  has rumdl && return 0
  local ver="${RUMDL_VERSION}"
  if [[ "${IS_MAC}" == true ]]; then
    echo "  Installing rumdl (brew install rumdl)..."
    brew install rumdl
  else
    echo "  Installing rumdl ${ver} (direct download)..."
    local arch
    arch="$(uname -m)"
    wget -q "https://github.com/rvben/rumdl/releases/download/v${ver}/rumdl-v${ver}-${arch}-unknown-linux-gnu.tar.gz" -O "/tmp/rumdl-${ver}.tar.gz"
    tar -xzf "/tmp/rumdl-${ver}.tar.gz" -C /tmp
    sudo mv /tmp/rumdl /usr/local/bin/rumdl
    sudo chmod +x /usr/local/bin/rumdl
    rm -f "/tmp/rumdl-${ver}.tar.gz"
  fi
}

main() {
  detect_platform
  echo "==> Checking tools..."

  has g++ || install_pkg "g++" "gcc" "g++"
  has jq || install_pkg "jq" "jq" "jq"

  install_llvm_tools

  has cppcheck || install_pkg "cppcheck" "cppcheck" "cppcheck"
  has pmccabe || install_pkg "pmccabe" "pmccabe" "pmccabe"
  has cloc || install_pkg "cloc" "cloc" "cloc"
  has shellcheck || install_pkg "shellcheck" "shellcheck" "shellcheck"
  has gitleaks || install_pkg "gitleaks" "gitleaks" "gitleaks"
  has yamllint || install_pkg "yamllint" "yamllint" "yamllint"

  install_doxygen
  install_semgrep
  install_rumdl

  if [[ "${IS_LINUX}" == true ]]; then
    has lcov || install_pkg "lcov" "lcov" "lcov"
    has bc || install_pkg "bc" "bc" "bc"
  fi

  echo ""
  echo "==> All tools:"
  printf "  %-20s %s\n" "g++" "$(g++ --version 2>/dev/null | head -1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1 || echo 'missing')"
  printf "  %-20s %s\n" "clang-format" "$(clang-format --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo 'missing')"
  printf "  %-20s %s\n" "cppcheck" "$(cppcheck --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+' || echo 'missing')"
  printf "  %-20s %s\n" "doxygen" "$(doxygen --version 2>/dev/null || echo 'missing')"
  printf "  %-20s %s\n" "cloc" "$(cloc --version 2>/dev/null || echo 'missing')"
  printf "  %-20s %s\n" "shellcheck" "$(shellcheck --version 2>/dev/null | grep '^version:' | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo 'missing')"
  printf "  %-20s %s\n" "yamllint" "$(yamllint --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo 'missing')"
  printf "  %-20s %s\n" "rumdl" "$(rumdl version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo 'missing')"
  printf "  %-20s %s\n" "semgrep" "$(semgrep --version 2>/dev/null || echo 'missing')"
  printf "  %-20s %s\n" "gitleaks" "$(has gitleaks && echo 'installed' || echo 'missing')"

  if [[ -d .git ]] && [[ -d scripts/git ]]; then
    echo "==> Installing git hooks..."
    [[ -f scripts/git/pre-commit.sh ]] && cp scripts/git/pre-commit.sh .git/hooks/pre-commit && chmod +x .git/hooks/pre-commit
    [[ -f scripts/git/pre-push.sh ]] && cp scripts/git/pre-push.sh .git/hooks/pre-push && chmod +x .git/hooks/pre-push
  fi

  echo "==> Setup complete. Run 'make check' to verify."
}

main "$@"
