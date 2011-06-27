include $(TOPDIR)/rules.mk

PKG_NAME:=injector
PKG_RELEASE:=2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
	
define Package/injector
  SECTION:=MyMANET
  CATEGORY:=MyMANET
  TITLE:=Injector for MyMANET
endef

define Package/injector/description
 This contains the injector file, which is a GNSM component.
endef

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/injector/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/inject $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,injector))
