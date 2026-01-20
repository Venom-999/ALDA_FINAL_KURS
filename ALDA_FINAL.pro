QT += quick

SOURCES += \
        catalog.cpp \
        datamanager.cpp \
        favorites.cpp \
        main.cpp \
        message.cpp \
        profile.cpp \
        request.cpp \
        review.cpp \
        service.cpp \
        subscription.cpp \
        user.cpp

resources.files = main.qml
resources.prefix = /ALDA_FINAL
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    catalog.h \
    datamanager.h \
    favorites.h \
    message.h \
    profile.h \
    request.h \
    review.h \
    service.h \
    subscription.h \
    user.h
