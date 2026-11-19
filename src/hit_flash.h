#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace ormenos {
void initialize_hit_flashes(ID3D11Device* device, ID3D11DeviceContext* context);
void detach_hit_flash_resources_on_process_exit();
void expire_hit_flashes();
}
