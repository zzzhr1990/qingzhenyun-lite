//
// Created by herui on 2019/11/30.
//

#include <qingzhen/api/file.h>

qingzhen::api::file::file() : identity(utility::string_t()),
                              hash(utility::string_t()),
                              user_identity(0),
                              path(utility::string_t()),
                              name(utility::string_t()),
                              ext(utility::string_t()),
                              size(0),
                              human_read_size(utility::string_t()),
                              mime(utility::string_t()),
                              deleted(false),
                              root(false),
                              parent(utility::string_t()),
                              type(0),
                              directory(false),
                              access_time(0),
                              create_time(0),
                              modify_time(0),
                              version(0),
                              locking(false),
                              op(0),
                              preview(false),
                              preview_type(0),
                              flag(0),
                              unique_identity(utility::string_t()),
                              share(false),
                              download_address(utility::string_t()) {

}

bool qingzhen::api::file::parse(const web::json::value &v) {
    // std::cout << "____SER: " << v.serialize().c_str() << std::endl;
    if (!v.has_string_field(_XPLATSTR("identity"))) {
        return false;
    }
    this->identity = v.at(_XPLATSTR("identity")).as_string();
    // 2.Hash
    if (!v.has_string_field(_XPLATSTR("hash"))) {
        return false;
    }
    this->hash = v.at(_XPLATSTR("hash")).as_string();
    if (!v.has_number_field(_XPLATSTR("userIdentity"))) {
        return false;
    }
    this->user_identity = v.at(_XPLATSTR("userIdentity")).as_number().to_int64();

    if (!v.has_string_field(_XPLATSTR("path"))) {
        return false;
    }
    const auto p = v.at(_XPLATSTR("path")).as_string();
    this->path = p;
    if (p == _XPLATSTR("/") || p.empty()) {
        this->root = true;
    }
    if (!v.has_string_field(_XPLATSTR("name"))) {
        return false;
    }
    this->name = v.at(_XPLATSTR("name")).as_string();
    if (this->root()) {
        this->name = _XPLATSTR("root");
    }
    if (!v.has_string_field(_XPLATSTR("ext"))) {
        return false;
    }
    this->ext = v.at(_XPLATSTR("ext")).as_string();
    if (!v.has_number_field(_XPLATSTR("size"))) {
        return false;
    }
    this->size = v.at(_XPLATSTR("size")).as_number().to_int64();
    if (!v.has_string_field(_XPLATSTR("mime"))) {
        return false;
    }
    this->mime = v.at(_XPLATSTR("mime")).as_string();
    if (!v.has_boolean_field(_XPLATSTR("deleted"))) {
        return false;
    }
    this->deleted = v.at(_XPLATSTR("deleted")).as_bool();
    if (!v.has_string_field(_XPLATSTR("parent"))) {
        return false;
    }
    this->parent = v.at(_XPLATSTR("parent")).as_string();
    if (!v.has_integer_field(_XPLATSTR("type"))) {
        return false;
    }
    this->type = v.at(_XPLATSTR("type")).as_integer();
    if (!v.has_boolean_field(_XPLATSTR("directory"))) {
        return false;
    }
    this->directory = v.at(_XPLATSTR("directory")).as_bool();
    if (!v.has_number_field(_XPLATSTR("atime"))) {
        return false;
    }
    this->access_time = v.at(_XPLATSTR("atime")).as_number().to_int64();
    if (!v.has_number_field(_XPLATSTR("ctime"))) {
        return false;
    }
    this->create_time = v.at(_XPLATSTR("ctime")).as_number().to_int64();
    if (!v.has_number_field(_XPLATSTR("mtime"))) {
        return false;
    }
    this->modify_time = v.at(_XPLATSTR("mtime")).as_number().to_int64();
    if (!v.has_number_field(_XPLATSTR("version"))) {
        return false;
    }
    this->version = v.at(_XPLATSTR("version")).as_number().to_int64();
    if (!v.has_boolean_field(_XPLATSTR("locking"))) {
        return false;
    }
    this->locking = v.at(_XPLATSTR("locking")).as_bool();
    if (!v.has_integer_field(_XPLATSTR("op"))) {
        return false;
    }
    this->op = v.at(_XPLATSTR("op")).as_integer();
    if (!v.has_boolean_field(_XPLATSTR("preview"))) {
        return false;
    }
    this->preview = v.at(_XPLATSTR("preview")).as_bool();
    if (!v.has_integer_field(_XPLATSTR("previewType"))) {
        return false;
    }
    this->preview_type = v.at(_XPLATSTR("previewType")).as_integer();
    if (!v.has_integer_field(_XPLATSTR("flag"))) {
        return false;
    }
    this->flag = v.at(_XPLATSTR("flag")).as_integer();
    if (!v.has_string_field(_XPLATSTR("uniqueIdentity"))) {
        return false;
    }
    this->unique_identity = v.at(_XPLATSTR("uniqueIdentity")).as_string();
    if (!v.has_boolean_field(_XPLATSTR("share"))) {
        return false;
    }
    this->share = v.at(_XPLATSTR("share")).as_bool();
    if (!v.has_string_field(_XPLATSTR("downloadAddress"))) {
        return false;
    }
    this->download_address = v.at(_XPLATSTR("downloadAddress")).as_string();
    return true;
}

web::json::value qingzhen::api::file::to_json() const {
    web::json::value res;
    res[_XPLATSTR("path")] = web::json::value(this->path());
    res[_XPLATSTR("identity")] = web::json::value(this->identity());
    res[_XPLATSTR("name")] = web::json::value(this->name());
    res[_XPLATSTR("hash")] = web::json::value(this->hash());
    return res;
}

