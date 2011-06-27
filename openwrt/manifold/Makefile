# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 8694 2007-09-08 19:55:42Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=manifold
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/manifold
  SUBMENU:=MyMANET
  TITLE:=MyMANET kernel module for mobile ad hoc networking
  FILES:=$(PKG_BUILD_DIR)/manifold.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,manifold)
endef

define KernelPackage/manifold/description
It is a kernel module used for mobile ad hoc networking.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,manifold))
