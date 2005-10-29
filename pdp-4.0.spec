Summary: The PDP++ software is a neural-network simulation system written in C++
Name: pdp
Version: 4.0
Release: 1
License: GNU Public License
Group: Computational Cognitive Neuroscience Lab
URL: http://psych.colorado.edu/~oreilly/PDP++/PDP++.html
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
The PDP++ software is a neural-network simulation system written in C++. It represents the next generation of the PDP software originally released with the McClelland and Rumelhart "Explorations in Parallel Distributed Processing Handbook", MIT Press, 1987. It is easy enough for novice users, but very powerful and flexible for research use.
%prep
%setup
./configure
%build
make
%install
make install
%clean
make clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README

%changelog
 
 
