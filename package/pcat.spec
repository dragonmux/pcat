Name: pcat
Version: 0.1.0
Release: 1%{?dist}
URL: https://github.com/DX-MON/pcat
Summary: A fast, parallel, file concatenation utility modelled after coreutils `cat`
License: GPLv3+

Source0: https://github.com/DX-MON/pcat/releases/v%{version}/pcat-%{version}.tar.xz
Source1: https://github.com/DX-MON/pcat/releases/v%{version}/%{name}-%{version}.tar.xz.asc

%description
A fast, parallel, file concatenation utility modelled after coreutils `cat`

BuildRequires: meson
BuildRequires: ninja-build >= 1.8.2
BuildRequires: gnupg2
Requires: pkgconfig
Requires: gcc >= 7
Requires: gcc-c++ >= 7
Requires: glibc-devel

%prep
gpg2 --no-default-keyring --keyring ./gpg-keyring.gpg --import-options import-minimal \
	--keyserver keys.gnupg.net --recv-keys 5E30DFF73626CA96F83DDF5E0137BA904EBDB4CD
gpg2 --keyring ./gpg-keyring.gpg --verify %{SOURCE1} %{SOURCE0}
rm ./gpg-keyring.gpg
%setup -q

%build
meson build --prefix=%{_prefix} --libdir=%{_libdir} --debug -Dstrip=false
ninja -C build

%check
ninja -C build test

%install
DESTDIR=$RPM_BUILD_ROOT ninja -C build install
export QA_RPATHS=0x0001

%clean
rm -r build

%files
%license COPYING
%{_bindir}/pcat
%{_mandir}/man1/pcat.1.gz

%changelog
* Mon Sept 21 2020 Rachel Mant <dx-mon@users.sourceforge.net> - 0.1.0-1
- Initial RPM packaging and release of pcat
