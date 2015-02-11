Name:           htop
Version:        1.0.4
Release:        1%{?dist}
Summary:        HTOP Linux Process Utility

Group:          Utility/Monitoring
License:        GPLv2
Source:		htop.tgz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%global debug_package %{nil}

%description
HTOP Linux Process Utility

%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
#make install DESTDIR=$RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/bin
cp -af htop $RPM_BUILD_ROOT/bin
cp -af sensors.sh $RPM_BUILD_ROOT/bin/sensors-htop

%clean
rm -rf $RPM_BUILD_ROOT

%post
if ! grep -RI "export HTOP_SENSORS_BINARY" /etc/bashrc > /dev/null 2>&1; then echo "export HTOP_SENSORS_BINARY=/bin/sensors-htop" >> /etc/bashrc; fi

%files
%defattr(-,root,root,-)
%doc
/bin/htop
/bin/sensors-htop

%changelog
