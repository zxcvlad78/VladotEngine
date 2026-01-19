#pragma once
#include "object/ref_counted/RefCounted.hpp"
#include <vector>
#include <string>

class Resource : public RefCounted {
public:
    std::string virtualPath;
    unsigned int rid = 0;

    Resource(std::string p_path) : virtualPath(std::move(p_path)) {}
    virtual ~Resource() override = default;

    virtual bool load_from_data(const std::vector<unsigned char>& data) = 0;
    virtual std::string get_class_name() const override { return "Resource"; }

    const std::string& get_path() const { return virtualPath; }
    unsigned int get_rid() const { return rid; }

};